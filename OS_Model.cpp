

//-------------------------------------------------------
//
//	İZMİR UNIVERSITY OF ECONOMICS
//	SE 311 - SOFTWARE ARCHITECTURE COURSE PROJECT
//	OPERATING SYSTEM - 06/2017
//	Onur  DURUKAN - Batuhan CANLITÜRK
//	
//-------------------------------------------------------


#include <iostream>
#include <string>
#include <vector>
#include <direct.h>
#include <stdio.h>
#include <windows.h>

using namespace std;

class OperatingSystem;
class ExternalDevice;
class ExternalDeviceCollection;
class FileSystem;

class Mutex {
};
class Lock {
public:
	Lock(Mutex& m) :
			mutex(m) {
	}
	~Lock() {
	}
private:
	Mutex & mutex;
};

// Iterator - AbstractIterator
class Iterator {
public:
	virtual void First() = 0;
	virtual void Next() = 0;
	virtual bool IsDone() const = 0;
	virtual ExternalDevice* CurrentItem() const = 0;
protected:
	Iterator() {
	}
	;
	virtual ~Iterator() {
	}
};

//Iterator - ConcreteIterator
class ExternalDeviceIterator: public Iterator {
public:
	~ExternalDeviceIterator() {
	}
	ExternalDeviceIterator(ExternalDeviceCollection *_devCol) :
			devCol(_devCol), _current(0) {
	}

	void First() {
		_current = 0;
	}
	void Next() {
		_current++;
	}
	ExternalDevice* CurrentItem() const;
	bool IsDone() const;

private:
	const ExternalDeviceCollection *devCol;
	int _current;
};

// Iterator - AbstractAggregate
class Collection {
public:
	virtual ~Collection() {
	}
	;
	virtual Iterator* CreateIterator() = 0;
	virtual void add(ExternalDevice *) = 0;
	virtual int getCount() const = 0;
	virtual ExternalDevice * get(int) const = 0;
protected:
	Collection() {}	;
};

//Iterator - ConcreteAggregate
class ExternalDeviceCollection: public Collection {
private:
	vector<ExternalDevice*> _ExternalDevices;
public:
	ExternalDeviceCollection() {
	}

	ExternalDeviceIterator* CreateIterator() {
		return new ExternalDeviceIterator(this);
	}
	int getCount() const {
		return _ExternalDevices.size();
	}
	void add(ExternalDevice *dev) {
		_ExternalDevices.push_back(dev);
	}
	ExternalDevice* get(int index) const {
		return _ExternalDevices[index];
	}

};

ExternalDevice *ExternalDeviceIterator::CurrentItem() const {
	return (IsDone() ? NULL : devCol->get(_current));
}
bool ExternalDeviceIterator::IsDone() const {
	return _current >= devCol->getCount();
}

// Composite - Component
class FileSystem {
public:
	FileSystem() {
	}
	virtual ~FileSystem() {
	}
	virtual void Add(FileSystem *fs, string path) {
	}
	virtual void Remove(string fs, string path) {
	}
	virtual void Display() {
	}
	virtual string Type()=0;
	string getName() {
		return name;
	}

protected:
	FileSystem(string name) :
			name(name) {
	}
	string name;

};

// AbstractFactory - AbstractProduct A
//Composite - Leaf
class File: public FileSystem {
public:
	File() {
	}
	File(string n) :
			FileSystem(n) {
	}
	virtual ~File() {
	}

	string Type() {
		return "file";
	}

	void Add(FileSystem *c) {
		cout << "Cannot add to a File\n";
	}
	void Remove(string c) {
		cout << "Cannot remove from a File \n";
	}
	void Display() {
		for (int i = 1; i <= 4; i++) {
			cout << "-";
		}
		cout << " " << getName() << endl;
	}
};

// AbstractFactory - ConcreteProduct A1-2-3
// Adapter - Adaptee
class LinuxFile: public File {
public:
	LinuxFile() {
	}
	LinuxFile(string name) :
			File(name) {
	}
	int uprintf(const char *str, FILE *handle) {
		fprintf(handle, str);
		fclose(handle);
		return 1;
	}

};
class BSDFile: public File {
public:
	BSDFile() {
	}
	BSDFile(string name) :
			File(name) {
	}

	int uprintf(const char *str, FILE *handle) {
		fprintf(handle, str);
		fclose(handle);
		return 1;
	}

};
class NTFile: public File {
public:
	NTFile() {
	}
	NTFile(string name) :
			File(name) {
	}

	int wprintf(const char *str, FILE *handle, int *rc) {
		fprintf(handle, str);
		fclose(handle);
		return *rc;
	}

};

// Adapter - Target
class InputOutput {
public:
	virtual int printf(FILE *file, const char *text)=0;
	virtual ~InputOutput() {
	}
};

// Adapter - Adapter
class Linux_PrintfConnector: public InputOutput, private LinuxFile {
public:
	int printf(FILE *file, const char *text) {
		uprintf(text, file);
		return 1;
	}
};
class NT_PrintfConnector: public InputOutput, private NTFile {
public:
	int printf(FILE *file, const char *text) {
		int val_rc = 1;
		int* rc = &val_rc;
		wprintf(text, file, rc);
		return 1;
	}
};
class BSD_PrintfConnector: public InputOutput, private BSDFile {
public:
	int printf(FILE *file, const char *text) {
		uprintf(text, file);
		return 1;
	}
};

// Composite - Composite
// AbstractFactory - AbstractProduct B
class Directory: public FileSystem {
public:
	Directory(string name) :
			FileSystem(name) {
		io = NULL;
	}

	string Type() {
		return "directory";
	}

	void Add(FileSystem* d, string path) {
		myFiles.push_back(d);
		string temp = path + "/" + d->getName();
		if (d->Type() == "file") {
			temp = temp + ".txt";
			FILE * myFile;
			myFile = fopen(temp.c_str(), "w");

			cout << "Text to " << d->getName() << " >> " << endl;
			char input[200];

			cin.ignore();
			cin.getline(input, 200);

			io->printf(myFile, input);

		} else {
			mkdir(temp.c_str());
		}
	}
	void Remove(string name, string path) {
		bool check = false;
		for (unsigned int i = 0; i < myFiles.size(); i++) {
			if (myFiles[i]->getName() == name) {
				delete myFiles[i];
				cout << name << " file was deleted" << endl;
				check = true;

				string temp = path + "/" + name;
				remove(temp.c_str());
			}
		}

		if (check == false)
			cout << "File was not found" << endl;
	}
	void Display() {
		for (int i = 1; i <= 4; i++) {
			cout << "-";
		}
		cout << "+ " + getName() << endl;

		for (unsigned int i = 0; i < myFiles.size(); i++) {
			myFiles[i]->Display();
		}
	}
	void Info(string name) {
		bool check = false;
		for (unsigned int i = 0; i < myFiles.size(); i++) {
			if (myFiles[i]->getName() == name) {
				cout << "File name is : " << myFiles[i]->getName() << endl;
				cout << "File size is : " << myFiles[i]->getName() << endl;
				cout << name << " is a " << myFiles[i]->Type() << endl;
				check = true;
			}
		}

		if (check == false) {
			cout << "File was not found" << endl;
		}
	}

protected:
	InputOutput *io;
	vector<FileSystem*> myFiles;
};

// AbstractFactory - ConcreteProduct B1-2-3
class LinuxDir: public Directory {
public:
	LinuxDir(string name) :
			Directory(name) {
		io = new Linux_PrintfConnector();
	}

};
class BSDDir: public Directory {
public:
	BSDDir(string name) :
			Directory(name) {
		io = new BSD_PrintfConnector();
	}
};
class NTDir: public Directory {
public:
	NTDir(string name) :
			Directory(name) {
		io = new NT_PrintfConnector();
	}
};

class InternalDevice {
public:
	virtual bool DeviceReset()=0;
	virtual ~InternalDevice() {
	}
};

// Facade - SubsystemClasses
class CPU: public InternalDevice {
public:
	bool DeviceReset() {
		cout << "All process is terminated." << endl;
		return true;
	}
};
class HardDisk: public InternalDevice {
public:
	bool DeviceReset() {
		cout << "Data is written to the buffer and all files are closed."
				<< endl;
		return true;
	}
};
class IO: public InternalDevice {
public:
	bool DeviceReset() {
		cout << "Input output ExternalDevices are closed." << endl;
		return true;
	}
};

//Singleton
//AbstractFactory - AbstractFactory
//Observer - AbstractObserver
// Facade - Facade
class OperatingSystem {
public:
	static OperatingSystem *GetOS();					   	// SINGLEON METHOD
	virtual void createFile(string name)=0; 		// ABSTRACT FACTORY METHOD
	virtual void createDirectory(string name)=0;
	virtual void Update(ExternalDevice* dev, string status)=0;  //OBSERVER METHOD

	void ShutDown() {											// FACADE METHOD
		bool readyForShutDown = true;

		if (!cpu->DeviceReset()) {
			readyForShutDown = false;
		} else if (!harddisk->DeviceReset()) {
			readyForShutDown = false;
		} else if (!io->DeviceReset()) {
			readyForShutDown = false;
		}

		if (readyForShutDown == true)
			cout << "Operating System is shutting down..." << endl;
		else
			cout << "There was a problem. Shutting down failed." << endl;
	}
	ExternalDeviceCollection* getDevices() {
		return myExternalDevices;
	}
	FileSystem* getRoot() {
		return root;
	}
	virtual string getPath()=0;

protected:
	virtual ~OperatingSystem() {
	}
	OperatingSystem(FileSystem* _root) {

		myExternalDevices = new ExternalDeviceCollection();
		ExternalDeviceIterator = myExternalDevices->CreateIterator();
		root = _root;
		cpu = new CPU();
		harddisk = new HardDisk();
		io = new IO();
	}

	OperatingSystem(const OperatingSystem&);
	OperatingSystem& operator=(const OperatingSystem&);
	static Mutex mutex;
	static OperatingSystem *OS;

	ExternalDeviceCollection* myExternalDevices;
	ExternalDeviceIterator* ExternalDeviceIterator;

	FileSystem* root;

	CPU *cpu;
	HardDisk *harddisk;
	IO *io;

};

// AbstractFactory - ConcreteFactory 1-2-3
//Observer - ConcreteObserver
class Linux: public OperatingSystem {
public:
	static OperatingSystem *GetOS() {
		if (OS == NULL) {
			Lock lock(mutex);
			if (OS == NULL) {
				OS = new Linux();
			}
		}
		return OS;
	}
	void createDirectory(string name) {
		root->Add(new LinuxDir(name), getPath());
		cout << name << " directory is created in " << getPath() << endl;
	}
	void createFile(string name) {
		root->Add(new LinuxFile(name), getPath());
		cout << name << " file is created in " << getPath() << endl;
	}
	void Update(ExternalDevice* dev, string status);
	string getPath() {
		return path;
	}

private:
	string path;
	Linux() :
			OperatingSystem(new LinuxDir("c:/Linux_Root")) {
		path = "c:/Linux_Root";
		mkdir(path.c_str());
	}
};

class BSD: public OperatingSystem {
public:
	static OperatingSystem *GetOS() {
		if (OS == NULL) {
			Lock lock(mutex);
			if (OS == NULL) {
				OS = new BSD();
			}
		}
		return OS;
	}

	void createDirectory(string name) {
		root->Add(new BSDDir(name), getPath());
		cout << name << " directory is created in " << getPath() << endl;
	}
	void createFile(string name) {
		root->Add(new BSDFile(name), getPath());
		cout << name << " file is created in " << getPath() << endl;
	}
	void Update(ExternalDevice* dev, string status);
	string getPath() {
		return path;
	}

private:
	string path;
	BSD() :
			OperatingSystem(new BSDDir("root")) {
		path = "c:/BSD_Root";
		mkdir(path.c_str());
	}
};

class NT: public OperatingSystem {
public:
	static OperatingSystem *GetOS() {
		if (OS == NULL) {
			Lock lock(mutex);
			if (OS == NULL) {
				OS = new NT();
			}
		}
		return OS;
	}

	void createDirectory(string name) {
		root->Add(new NTDir(name), getPath());
		cout << name << " directory is created in " << getPath() << endl;
	}
	void createFile(string name) {
		root->Add(new NTFile(name), path);
		cout << name << " file is created in " << getPath() << endl;
	}
	void Update(ExternalDevice* dev, string status);
	string getPath() {
		return path;
	}

private:
	string path;
	NT() :
			OperatingSystem(new NTDir("root")) {
		path = "c:/NT_Root";
		mkdir(path.c_str());
	}
};

// Observer - Subject
class ExternalDevice {
public:
	ExternalDevice(string _name, string _type) {
		name = _name;
		type = _type;
		OS = NULL;
	}
	virtual ~ExternalDevice() {
	}

	virtual void plugIn(OperatingSystem* os)=0;
	virtual void plugOut()=0;
	virtual void Notify(string reason)=0;

	string getName() {
		return name;
	}
	string getType() {
		return type;
	}

protected:
	string name;
	string type;
	OperatingSystem* OS;
};

// Observer - ConcreteSubject
class USB: public ExternalDevice {
public:
	~USB() {
	}
	USB(string name) :
			ExternalDevice(name, "USB") {
	}

	void plugIn(OperatingSystem* os) {
		OS = os;
		Notify("plugin");
	}
	void plugOut() {
		Notify("plugout");
	}
	void Notify(string reason) {
		OS->Update(this, reason);
	}

};

class HDMI: public ExternalDevice {
public:
	HDMI(string name) :
			ExternalDevice(name, "HDMI") {
	}

	void plugIn(OperatingSystem* os) {
		OS = os;
		Notify("plugin");
	}
	void plugOut() {
		Notify("plugout");
	}
	void Notify(string reason) {
		OS->Update(this, reason);
	}

};
class HeadPhone: public ExternalDevice {
public:
	HeadPhone(string name) :
			ExternalDevice(name, "HeadPhone") {
	}

	void plugIn(OperatingSystem* os) {
		OS = os;
		Notify("plugin");
	}
	void plugOut() {
		Notify("plugout");
	}
	void Notify(string reason) {
		OS->Update(this, reason);
	}

};

void Linux::Update(ExternalDevice *dev, string status = "plugin") {
	if (status == "plugin") {
		myExternalDevices->add(dev);

		Beep(600,500);
		cout << dev->getName() << " " << dev->getType() << " is plugged in."
				<< endl;

	} else if (status == "plugout") {
		for (ExternalDeviceIterator->First(); !ExternalDeviceIterator->IsDone();
				ExternalDeviceIterator->Next()) {
			if (ExternalDeviceIterator->CurrentItem() == dev) {
				delete ExternalDeviceIterator->CurrentItem();

				Beep(523,500);
				cout << dev->getType() << " was plugged out " << endl;
			}
		}
	}
}

void BSD::Update(ExternalDevice *dev, string status = "plugin") {
	if (status == "plugin") {
		myExternalDevices->add(dev);

		Beep(600,500);
		cout << dev->getName() << " " << dev->getType() << " is plugged in."
				<< endl;

	} else if (status == "plugout") {
		for (ExternalDeviceIterator->First(); !ExternalDeviceIterator->IsDone();
				ExternalDeviceIterator->Next()) {
			if (ExternalDeviceIterator->CurrentItem() == dev) {
				delete ExternalDeviceIterator->CurrentItem();

				Beep(523,500);
				cout << dev->getType() << " was plugged out " << endl;
			}
		}
	}
}

void NT::Update(ExternalDevice *dev, string status = "plugin") {
	if (status == "plugin") {
		myExternalDevices->add(dev);

		Beep(600,500);
		cout << dev->getName() << " " << dev->getType() << " is plugged in."
				<< endl;

	} else if (status == "plugout") {
		for (ExternalDeviceIterator->First(); !ExternalDeviceIterator->IsDone();
				ExternalDeviceIterator->Next()) {
			if (ExternalDeviceIterator->CurrentItem() == dev) {
				delete ExternalDeviceIterator->CurrentItem();

				Beep(523,500);
				cout << dev->getType() << " was plugged out " << endl;
			}
		}
	}
}

Mutex OperatingSystem::mutex;
OperatingSystem *OperatingSystem::OS = 0;

void Operate(OperatingSystem* OS, string type) {
	Beep(700,700);
	Beep(700,700);

	cout << type << " is ready !" << endl;
	cout << "------------------------------" << endl;
	cout << "WELCOME AGAIN !" << endl;
	cout << "------------------------------" << endl;

	string username;
	cout << "Username: ";
	cin >> username;
	cout << endl;

	cout << "For help, type 'help' " << endl;
	string command = " ";
	while (command != "exit") {
		cout << username << "@" << type << " $" << " >> ";
		cin >> command;
		cout << endl;

		if (command == "fopen") {
			cout << "File name >> ";
			string fname;
			cin >> fname;

			OS->createFile(fname);

		} else if (command == "mkdir") {
			cout << "Directory name >> ";
			string dname;
			cin >> dname;

			OS->createDirectory(dname);

		} else if (command == "remove") {
			cout << "File name >> ";
			string rname;
			cin >> rname;

			OS->getRoot()->Remove(rname, OS->getPath());

		} else if (command == "mount") {
			cout << "(1)USB		(2)HDMI		(3)HeadPhone >> ";
			int device;
			cin >> device;
			cout << endl;

			cout << "Device name >> ";
			string devName;
			cin >> devName;
			cout << endl;

			switch (device) {
			case 1: {
				ExternalDevice *dev = new USB(devName);
				dev->plugIn(OS);
				break;
			}
			case 2: {
				ExternalDevice *dev = new HDMI(devName);
				dev->plugIn(OS);
				break;
			}
			case 3: {
				ExternalDevice *dev = new HeadPhone(devName);
				dev->plugIn(OS);
				break;
			}
			}

		} else if (command == "unmount") {
			cout << "Device name: ";
			string devName;
			cin >> devName;
			cout << endl;

			bool check = false;
			for (int i = 0; i < OS->getDevices()->getCount(); i++) {
				if (devName == OS->getDevices()->get(i)->getName()) {
					OS->getDevices()->get(i)->plugOut();
					check = true;
				}
			}
			if (check == false)
				cout << "Device called " << devName << " was not found" << endl;

		} else if (command == "list") {
			OS->getRoot()->Display();

		} else if (command == "help") {
			cout << "Commands" << endl;
			cout << "---------------------------------------------" << endl;
			cout << "fopen		Create file" << endl;
			cout << "mkdir		Create directory" << endl;
			cout << "mount		Plug device" << endl;
			cout << "remove		Delete all files and directories" << endl;
			cout << "unmount		Unplug device" << endl;
			cout << "list		List files and directories" << endl;
			cout << "clear		Clear the console" << endl;
			cout << "exit		Shut down the system" << endl;
			cout << "---------------------------------------------" << endl;
		} else if (command == "exit") {
			OS->ShutDown();
		} else if (command == "clear") {
			for (int n = 0; n < 10; n++)
				printf("\n\n\n\n\n\n\n\n\n\n\n\n\n");
		} else
			cout << "Command not found." << endl;

	}
}

int main() {
	int choice;

	do {
		cout << "Choose your operating system" << endl;
		cout << "____________________________" << endl;
		cout << " (1) Linux" << endl;
		cout << " (2) BSD" << endl;
		cout << " (3) NT" << endl;
		cout << "             " << endl;
		cout << " (0) Exit" << endl;
		cout << "____________________________" << endl;
		cout << "> ";

		cin >> choice;

		switch (choice) {
		case 0:
			cout << "-------System aborted--------";
			break;
		case 1: {
			OperatingSystem *OS = Linux::GetOS();
			Operate(OS, "Linux");
			break;
		}
		case 2: {
			OperatingSystem *OS = BSD::GetOS();
			Operate(OS, "BSD");
			break;
		}
		case 3: {
			OperatingSystem *OS = NT::GetOS();
			Operate(OS, "NT");
			break;
		}

		default:
			cout << "Please enter a valid choice" << endl;
		}

	} while (choice != 0);

	return 0;
}

