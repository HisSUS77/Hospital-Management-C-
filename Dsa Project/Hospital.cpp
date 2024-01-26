#include <iostream>
#include <stack>
#include <queue>
#include <fstream>
#include <sstream>
#include <limits>
#include <vector>
#include <algorithm> 

using namespace std;

class Time {
public:
    int hours;
    int minutes;

    Time(int h, int m) : hours(h), minutes(m) {}

    bool operator>(const Time& other) const {
        return (hours > other.hours) || (hours == other.hours && minutes > other.minutes);
    }

    bool operator<(const Time& other) const {
        return (hours < other.hours) || (hours == other.hours && minutes < other.minutes);
    }

    void displayTime() {
        cout << "Time : " << hours << ": " << minutes << endl;
    }
};

class Date {
public:
    int day;
    int month;

    Date(int d, int m) : day(d), month(m) {}

    void displayDay() {
        cout << "Date : " << day << ", " << month;
    }

    bool CheckSameDate(const Date& d1) {
        return (day == d1.day) && (month == d1.month);
    }
};

class Patient {
public:
    static int nextID;

    string pname;
    int idnum;
    bool Emergency_status;

    Patient() : idnum(nextID++) {}

    Patient(const Patient& p1) : pname(p1.pname), idnum(nextID++), Emergency_status(p1.Emergency_status) {}
};

int Patient::nextID = 1;

Patient GetPatientDetails() {
    Patient patient;
    cout << "Enter patient name: ";
    getline(cin, patient.pname);

    cout << "Enter patient name: ";
    getline(cin, patient.pname);

    cout << "Is the patient in emergency? (1 for true, 0 for false): ";
    while (!(cin >> patient.Emergency_status) || (patient.Emergency_status != 0 && patient.Emergency_status != 1)) {
        cout << "Invalid input. Please enter 1 for true or 0 for false: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return patient;
}

void displayPatient(const Patient& p) {
    cout << "New Patient details:" << endl;
    cout << "ID Number: " << p.idnum << endl;
    cout << "Name: " << p.pname << endl;
    cout << "Emergency Status: " << (p.Emergency_status ? "Yes" : "No") << endl;
}

struct pnode {
    Patient p;
    pnode* next;

    pnode(const Patient& patient) : p(patient), next(nullptr) {}
};

class Plist {
public:
    pnode* ppp;

    Plist() : ppp(nullptr) {}

    void AddPatient(const Patient& pat) {
        if (ppp == nullptr) {
            ppp = new pnode(pat);
            return;
        }

        pnode* t = ppp;
        while (t->next != nullptr) {
            t = t->next;
        }

        t->next = new pnode(pat);
    }

    void displayPatientss() {
        if (ppp == nullptr) {
            return;
        }

        pnode* t = ppp;
        while (t != nullptr) {
            displayPatient(t->p);
            t = t->next;
        }
    }
};

class Doctor {
public:
    string dr_name;
    string specialization;
    string shift;
    stack<Time> Timings;

    Doctor() {}

    Doctor(string name, string spec, string sh) : dr_name(name), specialization(spec), shift(sh) {
        Timings = Timeslots();
    }

    void UpdateAvailableTimeSlots() {
        if (!Timings.empty()) {
            Timings.pop();
        }
    }

    stack<Time> Timeslots() {
        stack<Time> result;

        if (shift == "Day") {
            result.push(Time(12, 0));
            result.push(Time(13, 0));
            result.push(Time(14, 0));
            result.push(Time(15, 0));
        } else if (shift == "Morning") {
            result.push(Time(6, 0));
            result.push(Time(7, 0));
            result.push(Time(8, 0));
            result.push(Time(9, 0));
        } else if (shift == "Evening") {
            result.push(Time(17, 0));
            result.push(Time(18, 0));
            result.push(Time(19, 0));
            result.push(Time(20, 0));
        } else {
            // for night shift
            result.push(Time(1, 0));
            result.push(Time(2, 0));
            result.push(Time(3, 0));
            result.push(Time(4, 0));
        }

        return result;
    }
};

struct drnode {
    Doctor d;
    drnode* next;

    drnode(const Doctor& dr) : d(dr), next(nullptr) {}
};

class drlist {
public:
    drnode* p;

    drlist() : p(nullptr) {}

    void readDoctorsFromFile(const string& filename) {
        ifstream file(filename);

        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string name, specialization, shift;

            if (ss >> name >> specialization >> shift) {
                Doctor newDoctor(name, specialization, shift);
                drnode* newNode = new drnode(newDoctor);
                newNode->next = p;
                p = newNode;
            } else {
                cerr << "Error parsing line: " << line << endl;
            }
        }

        file.close();
    }

    void displayDoctorsInfo() {
        if (p == nullptr) {
            cout << "Extremely sorry, but, No doctors available right now." << endl;
            return;
        }

        drnode* current = p;
        while (current != nullptr) {
            cout << "Doctor Name: " << current->d.dr_name << endl;
            cout << "Doctors Specialization: " << current->d.specialization << endl;

            stack<Time> tempTimings = current->d.Timings;
            while (!tempTimings.empty()) {
                cout << tempTimings.top().hours << ":" << tempTimings.top().minutes << endl;
                tempTimings.pop();
            }

            current = current->next;
        }
    }

};

class Appointment {
public:
    int appointment_id;
    string dr_name;
    string patient;
    Time timeSlot;

    Appointment() : appointment_id(0), dr_name(""), patient(""), timeSlot(0, 0) {}

    Appointment(Doctor* d, Patient& p) : appointment_id(0), dr_name(""), patient(""), timeSlot(0, 0) {
        static int next_id = 1;
        appointment_id = next_id++;
        timeSlot = GetAppointmentTime(d);
        patient = p.pname;
        dr_name = d->dr_name;

        // Update the doctor's available time slots
        d->UpdateAvailableTimeSlots();
    }
    Time GetAppointmentTime(Doctor* d) {
        if (!d->Timings.empty()) {
            return d->Timings.top();
        } else {
            return Time(0, 0); // Default time if Timings stack is empty
        }
    }

    void writeAppointmentToFile(const string& filename, const Appointment& appointment) {
        ofstream file(filename, ios::app); // Open the file in append mode

        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }

        file << appointment.appointment_id << " "
             << appointment.dr_name << " "
             << appointment.patient << " "
             << appointment.timeSlot.hours << ":" << appointment.timeSlot.minutes << endl;

        if (file.fail()) {
            cerr << "Error writing to file: " << filename << endl;
        }

        file.close();
    }



};

    void displayAppointment(Appointment app) {
        cout << "\n\t Appointment ID: " << app.appointment_id << endl;
        cout << "\n\t Allocated Doctor: " << app.dr_name << endl;
        cout << "\n\t Patient Name: " << app.patient << endl;
        cout << "\n\t Time Slot: " << app.timeSlot.hours << "  " << app.timeSlot.minutes << endl;
    }
    
struct Anode {
    Appointment A;
    Anode* next;

    Anode(Appointment& app) : A(app), next(nullptr) {}
};


class Applist{
   
public:

Anode* a;

Applist() : a(nullptr) {}

void InsertAppointment(Appointment appointment){
     Anode* newNode = new Anode(appointment);
    
     
        if(a == nullptr){
             a = newNode;
        }

    else {

         Anode* t = a ;
    while(t->next != nullptr){
        t = t->next;
    }

    t->next = newNode;


    }
  
}

void DisplayALLappointments(){

    if(a == nullptr ){
        return;
    }
    Anode* t  = a ;
    while(t!= nullptr){
        displayAppointment(t->A);
        t = t->next;
    }
}

void DeleteAppointment(int id){
    if(a == nullptr){
        cout<< "\t \t no Appointments yet ";
        return;
    }
    
     Anode* t  = a ;
    Anode* f = a ; 
     if(t->A.appointment_id == id){
        f = t->next;
        free(t);
        a = f;
        return;
     }

    while(t != nullptr){
        if(t->next->A.appointment_id == id){
            f = t->next;
            t->next = f->next;
            free(f);
            cout<< "\t Appointment Successfully deleted "<<endl;
            return;
        }
        t = t->next;
    }
 
}



    void executeAppointment(){
        if(a==nullptr){
            cout<<"no Appointments yet";
        }

    Anode* t  = a ;
    Anode* f = a ; 
        
    while(t != nullptr){

            f = t->next;
            displayAppointment(t->A);
            free(t);
            t = f;  
            cout<< "\t Appointment Successfully Executed "<<endl;
            return;
        }
        t = t->next;
    }



    


};

int main() {
    Applist Appointments;
    drlist doctors;
    Plist patients;

    int choice;
    do {
        cout << "\n=== Welcome To AL-Shifa (Charitable) Hospital ===" << endl;
        cout << "1. Enter New Patient" << endl;
        cout << "2. Display Doctors Info" << endl;
        cout << "3. Set Appointment" << endl;
        cout << "4. Display Appointments" << endl;
        cout << "5. Display all patients info " << endl;
        cout << "6. Delete an Appointment " << endl;
        cout<< "7. Execute Appointments "<<endl;
        cout << "8. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                // Enter New Patient
                Patient newPatient = GetPatientDetails();
                patients.AddPatient(newPatient);
                // Do something with the new patient details, e.g., display them
                displayPatient(newPatient);
                break;
            }
            case 2:
                // Display Doctors Info
                doctors.readDoctorsFromFile("E:\\Desktop\\dsa project\\Doctors.txt");
                doctors.displayDoctorsInfo();
                break;
            case 3: {
                // Set Appointment
                cout << "\nEnter 1 for Cardiologist";
                cout << "\nEnter 2 for Orthopedic";
                cout << "\nEnter 3 for Neurosurgeon";
                cin >> choice;

                drnode* current = doctors.p;
                Doctor* c = nullptr;
                bool Found = false;

                switch (choice) {
                    case 1:
                        while (current != nullptr) {
                            if (current->d.specialization == "Cardiologist") {
                                Found = true;
                                c = &(current->d);
                                break;
                            }
                            current = current->next;
                        }
                        break;
                    case 2:
                        while (current != nullptr) {
                            if (current->d.specialization == "Orthopedic") {
                                Found = true;
                                c = &(current->d);
                                break;
                            }
                            current = current->next;
                        }
                        break;
                    case 3:
                        while (current != nullptr) {
                            if (current->d.specialization == "Neurosurgeon") {
                                Found = true;
                                c = &(current->d);
                                break;
                            }
                            current = current->next;
                        }
                        break;
                    default:
                        cout << "Invalid choice. Please enter a valid specialization." << endl;
                        continue; 
                }

                if (!Found) {
                    cout << "Sorry, the selected specialization is not found." << endl;
                } else {
                    Patient p = GetPatientDetails();
                    patients.AddPatient(p);
                    Appointment newAppointment(c, p);
                    Appointments.InsertAppointment(newAppointment);
                    cout << "Appointment ID: " << newAppointment.appointment_id << endl;
                    newAppointment.writeAppointmentToFile("Appointments.txt", newAppointment);
                }
                break;
            }
            case 4:
            
                
                Appointments.DisplayALLappointments();
                break;
            case 5:
                // display all patient Info :
                patients.displayPatientss();
                break;
            case 6:
            cout<<"\t Enter Appointment id to Delete an Appointment : " << endl;
                 int a ;
                cin>>a;
                    Appointments.DeleteAppointment(a);
                break;
            case 7:
            Appointments.executeAppointment();

            break;
            case 8:
                cout << "Exiting program. Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice. Please enter a valid option." << endl;
        }

    } while (choice != 8);

    return 0;
}
