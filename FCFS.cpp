#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <cstdlib>
using namespace std;

//*******************************************************
//Process Struct
//******************************************************
struct Process
{
	int CPU_Burst[10];
	int IO[9];
	int Arrival_Time;
	int Current_Burst; //records which element in the array the burst is at
	int Current_IO;		//records which element in the array the IO is at
	int Process_State;	//1 = waiting in ready queue, 2 = executing, 3 = IO, 4 = completed
	int Response_Time, Turnaround_Time, Wait_Time;
};

//******************************************
//Node class for use with Queue
//*****************************************
class qnode
{
public:
		string data;
		qnode *prev, *next;
};


//****************************************
//Queue class
//****************************************
class Queue
{
public:
	Queue();
	~Queue();
	bool Empty();
	void Enqueue(const string & value);
	void Dequeue();
	int Execution_FCFS(Process P[], int n, ofstream &out);
	void Print(ofstream &out);	//prints data in ready queue to console and output file
private:
	qnode *front;
};

void Initialize_Processes(Process P[], int n);
void Initialize_Ready_Queue(Queue &Q, Process P[], int n);

//*********************************
//Default Constructor
//*********************************
Queue::Queue()
{
	front = 0;
}


//*************************************************
//Add a single item to back of queue. Takes string argument
//**************************************************
void Queue::Enqueue(const string &value)
{
	if (Empty())
	{
		front = new qnode;
		front->data = value;
		front->next = front;
		front->prev = front;
	}
	else
	{
		qnode *p = new qnode;
		p->data = value;
		front->prev->next = p;
		p->next = front;
		p->prev = front->prev;
		front->prev = p;
	}
}

//*****************************************
//Remove single item from front of queue
//******************************************
void Queue::Dequeue()
{
	if (Empty())
	{
		cout << "Error: Queue is already empty.\n\n";
	}
	else if (front == front->prev)
	{
		front = 0;
	}
	else
	{
		qnode *p = front;
		front = front->next;
		front->prev = p->prev;
		delete p;
	}
}

//*********************************************
//Check if Queue is empty
//*********************************************
bool Queue::Empty()
{
	return front == 0;
}

//*******************************************
//Print function for queue class
//******************************************
void Queue::Print(ofstream &out)
{
	if (Empty())
	{
		cout << "              [Empty]                        \n\n";
		out << "              [Empty]                        \n\n";
		cout << "...............................................\n\n";
		out << "...............................................\n\n";
	}
	else
	{
		qnode *p = front;
		while (p != front->prev)
		{
			cout << p->data;
			out << p->data;
			p = p->next;
		}
		cout << p->data << endl;
		cout << "...............................................\n\n";
		out << p->data << endl;
		out << "...............................................\n\n";
	}
}


//******************************************
//Destructor for Queue class
//******************************************
Queue::~Queue()
{
	while (!Empty())
	{
		Dequeue();
	}
}

//**************************************************************
//Initialize the 8 processes using Struct
//**************************************************************
void Initialize_Processes(Process P[], int n)
{
	P[0] = { {4, 5, 6, 7, 6, 4, 5, 4}, { 15, 31, 26, 24, 41, 51, 16 }, 0, 0, 0, 1, 0, 0, 0 };
	P[1] = { {9, 11, 15, 12, 8, 11, 9, 10, 7}, {28, 22, 21, 28, 34, 34, 29, 31}, 0, 0, 0, 1, 0, 0, 0 };
	P[2] = { {24, 12, 6, 17, 11, 22, 18}, {28, 21, 27, 21, 54, 31}, 0, 0, 0, 1, 0, 0, 0 };
	P[3] = { {15, 14, 16, 18, 14, 13, 16, 15}, {35, 41, 45, 51, 61, 54, 61}, 0, 0, 0, 1, 0, 0, 0 };
	P[4] = { {6, 5, 15, 4, 7, 4, 6, 10, 3}, {22, 21, 31, 26, 31, 18, 21, 33}, 0, 0, 0, 1, 0, 0, 0 };
	P[5] = { {22, 27, 25, 11, 19, 18, 6, 6}, {38, 41, 29, 26, 32, 22, 26}, 0, 0, 0, 1, 0, 0, 0 };
	P[6] = { {4, 7, 6, 5, 4, 7, 6, 5, 6, 9}, {36, 31, 32, 41, 42, 39, 33, 34, 21}, 0, 0, 0, 1, 0, 0, 0 };
	P[7] = { {5, 4, 6, 4, 6, 5, 4, 6, 6}, {14, 33, 31, 31, 27, 21, 19, 11}, 0, 0, 0, 1, 0, 0, 0 };
}

//********************************************************
//Initalize the ready queue by loading up the queue
//********************************************************
void Initialize_Ready_Queue(Queue &Q, Process P[], int n)
{
	for (int i = 0; i < 8; i++)
	{
		Q.Enqueue("               P" + to_string(i+1) + "          " + to_string(P[i].CPU_Burst[0]) + "\n");
	}
}

int Queue::Execution_FCFS(Process P[], int n, ofstream &out)
{
	
	qnode *p = front;			//point to front of queue
	string executing = p->data;	//retrieve data in front and assign to string executing
	Dequeue();					//run dequeue to remove process to be executed from ready queue

	//***************************************************************
	//split the string data between process number and cpu burst
	//***************************************************************
	istringstream ss(executing);	
	string token, processNumber;
	int Burst;
	int Current_Time = 0;
	int CPU_Utilization = 0;
		while (ss >> token)
		{
			if (!isdigit(token[0]))
			{
				processNumber = token[1];
			}
			stringstream(token) >> Burst;
		}

		//*********************************************************
		//convert process number to integer
		//*********************************************************
		int temp;
		stringstream(processNumber) >> temp;

		P[temp-1].Process_State = 2;		//assign process state to executing

		//****************************************************************
		//Begin printing output to console AND output file
		// out = ofstream variable
		//******************************************************************
		cout << "Current Time: " << Current_Time << endl << endl;
		out << "Current Time: " << Current_Time << endl << endl;

		cout << "Now running: P" << processNumber << endl;
		cout << "......................................................\n\n";
		cout << "Ready Queue:  Process     Burst\n";

		out << "Now running: P" << processNumber << endl;
		out << "......................................................\n\n";
		out << "Ready Queue:  Process     Burst\n";

		Print(out);

		cout << "Now in I/O:  Process     Remaining I/O Time\n";
		cout << "             [Empty]                        \n\n";
		
		cout << "......................................................\n";
		cout << "......................................................\n\n";

		out << "Now in I/O:  Process     Remaining I/O Time\n";
		out << "             [Empty]                        \n\n";

		out << "......................................................\n";
		out << "......................................................\n\n";

		//**************************************************
		//Run the first CPU burst
		//**************************************************
		for (int i = Burst; i > 0; i--)
		{
			Current_Time++;
			CPU_Utilization++;
			//Increment wait time for all processes in ready state
			for (int j = 0; j < n; j++)
			{
				if (P[j].Process_State == 1)
				{
					P[j].Wait_Time++;
				}
			}
		}

		//Change current Process' CPU burst to the next burst
		P[temp - 1].Current_Burst++;
		//Move last executing (first process) process to IO
		P[temp - 1].Process_State = 3;
	


		int completedFlag = 0;	//initialize completion flag for processes
		int IOflag = 0;			//initialize IO flag for IO "queue"
		int ignoreOnce = 0;		//flag to ignore completion output the first time, explained later

		//run loop until both the Ready Queue AND IO is empty
		while (!Empty() || IOflag == 1)
		{
			//Check to see if IO is empty for all processes, if so reset IO flag to 0
			if (P[0].Process_State != 3 && P[1].Process_State != 3 && P[2].Process_State != 3 && P[3].Process_State != 3 
				&& P[4].Process_State != 3 && P[5].Process_State != 3 && P[6].Process_State != 3 && P[7].Process_State != 3)
			{
				IOflag = 0;
			}

			//******************************************************
			//Extract front of the ready queue and assign to string
			qnode *p = front;
			string executing = p->data;

			istringstream ss(executing);
			string token, processNumber;
			Dequeue();
			//Remove from ready queue
			//*********************************************************


			//Parse string into two subfields
			//********************************************************
			while (ss >> token)
			{
				if (!isdigit(token[0]))
				{
					processNumber = token[1];
				}
				stringstream(token) >> Burst;
			}
				
			//*********************************************************
			//Convert processnumber to integer
			//*********************************************************
			int temp;
			stringstream(processNumber) >> temp;
			P[temp - 1].Process_State = 2;

			//Once process reaches state 2 for the first time, set to the response time - arrival time.
			//The way this code works, Process 1 is guaranteed to go first outside of while loop so ignore P1
			//code would need to be changed if arrival times were different
			if (P[temp - 1].Response_Time == 0 && (temp-1) != 0)
			{
				P[temp - 1].Response_Time = Current_Time - P[temp - 1].Arrival_Time; // arrival time is 0
			}


			//*******************************************************************
			//Display basic output to console AND output file (using out ofstream)
			//********************************************************************
			cout << "Current Time: " << Current_Time << endl << endl;
			out << "Current Time: " << Current_Time << endl << endl;

			cout << "Now running: P" << processNumber << endl;
			cout << "......................................................\n\n";
			cout << "Ready Queue:  Process     Burst\n";
			out << "Now running: P" << processNumber << endl;
			out << "......................................................\n\n";
			out << "Ready Queue:  Process     Burst\n";

			Print(out);

			cout << "Now in I/O:  Process     Remaining I/O Time\n";
			out << "Now in I/O:  Process     Remaining I/O Time\n";

			//********************************************************************
			//Check if IO has processes and print them if they exist
			//********************************************************************
			for (int k = 0; k < n; k++)
			{
				if (P[k].Process_State == 3)
				{
						cout << "                P" << k+1 << "                 " << P[k].IO[P[k].Current_IO] << endl;
						out << "                P" << k + 1 << "                 " << P[k].IO[P[k].Current_IO] << endl;
						IOflag = 1;
				}
			}
			if (IOflag == 0)
			{
				cout << "             [Empty]                        \n\n";
				out << "             [Empty]                        \n\n";
			}
			
			cout << "......................................................\n";
			cout << "......................................................\n\n";
			out << "......................................................\n";
			out << "......................................................\n\n";
			
			//*************************************
			//CPU BURST +  IO transition
			//*************************************
			for (int i = Burst; i > 0; i--)
			{
				Current_Time++;
				CPU_Utilization++;

				for (int j = 0; j < n; j++)
				{
					//Increment wait time for all processes in ready state
					if (P[j].Process_State == 1)
					{
						P[j].Wait_Time++;
					}
					//Move IO Process into ready queue if IO time expires
					if (P[j].IO[P[j].Current_IO] == 0 && P[j].Process_State == 3)
					{
						P[j].Current_IO++;
						P[j].Process_State = 1;
						Enqueue("               P" + to_string(j + 1) + "          " + to_string(P[j].CPU_Burst[P[j].Current_Burst]) + "\n");
						P[j].Wait_Time++;	//it's back in ready queue, need to add wait time
					}
					//decrement current IO time for any process in IO
					else if (P[j].IO[P[j].Current_IO] > 0 && P[j].Process_State == 3)
					{
						P[j].IO[P[j].Current_IO]--;
					}
				}
			}

			//Increment burst of last Process that just finished executing to the next burst
			P[temp - 1].Current_Burst++;
			//Check if last process has no more CPU burst left, mark completed if so
			if (P[temp-1].CPU_Burst[P[temp-1].Current_Burst + 1] == 0 && P[temp-1].CPU_Burst[P[temp-1].Current_Burst] == 0 && P[temp-1].Process_State != 4)
				{
					P[temp-1].Turnaround_Time = Current_Time - P[temp-1].Arrival_Time;	//arrival time is 0 so turnaround time is just the time it finishes
					P[temp-1].Process_State = 4;
					if (completedFlag == 0) //if complete flag hasn't been triggered yet, trigger it, otherwise skip code
					{
						completedFlag = 1;	//flag to mark completion
						ignoreOnce = 1;		//when first process completes, don't want to print until next time unit. Flag to skip printing first completion
					}
				}
			//otherwise send to IO
			else
			{
				P[temp - 1].Process_State = 3;
			}

			//*************************************************************
			//Check if there are completed processes and if so, print them
			//*************************************************************
			if (completedFlag == 1 && ignoreOnce == 0)
			{
				cout << "Completed Processes:\n";
				out << "Completed Processes:\n";

				for (int x = 0; x < n; x++)
				{
					if (P[x].Process_State == 4 && x != (temp-1))
					{
						cout << "     P" << x + 1 << "\n";
						out << "     P" << x + 1 << "\n";
					}
				}
				cout << "....................................................\n\n";
				out << "....................................................\n\n";
			}

			if (ignoreOnce == 1)
			{
				ignoreOnce = 0;		//ignore once needs to be reset after doing it's job.
			}

			//*********************************************************************
			//Check if ready queue is empty but there are still processes in the IO
			//***********************************************************************
			while (Empty() && IOflag == 1)
			{
				//*********************************************************
				//If we enter this while loop, print idle status
				//*********************************************************
				cout << "Current Time: " << Current_Time << endl << endl;
				out << "Current Time: " << Current_Time << endl << endl;

				cout << "Now running: [idle]" << endl;
				cout << "......................................................\n\n";
				cout << "Ready Queue:  Process     Burst\n";
				out << "Now running: [idle]" << endl;
				out << "......................................................\n\n";
				out << "Ready Queue:  Process     Burst\n";

				Print(out);

				cout << "Now in I/O:  Process     Remaining I/O Time\n";
				out << "Now in I/O:  Process     Remaining I/O Time\n";

				//********************************************************************
				//print IO 
				//********************************************************************
				for (int k = 0; k < n; k++)
				{
					if (P[k].Process_State == 3)
					{
						cout << "                P" << k + 1 << "                 " << P[k].IO[P[k].Current_IO] << endl;
						out << "                P" << k + 1 << "                 " << P[k].IO[P[k].Current_IO] << endl;
						IOflag = 1;
					}
				}

				cout << "....................................................\n\n";
				out << "....................................................\n\n";

				int NoExecutions = 0;	//flag to check if process state is in state 2 for any process

				//check if any processes are executing
				for (int j = 0; j < n; j++)
				{
					if (P[j].Process_State == 2)
					{
						NoExecutions = 0;
						break;
					}
					else
					{
						NoExecutions = 1;
					}
				}

				//if ready queue is empty, no processes are executing, and io still has processes then
				while (Empty() && IOflag == 1 && NoExecutions == 1)
				{
					//decrement each process in the IO by 1
					for (int j = 0; j < n; j++)
					{
						//Move Process into ready queue once it reaches expiration
						if (P[j].IO[P[j].Current_IO] == 0 && P[j].Process_State == 3)
						{
							P[j].Current_IO++;
							P[j].Process_State = 1;
							Enqueue("               P" + to_string(j + 1) + "          " + to_string(P[j].CPU_Burst[P[j].Current_Burst]) + "\n");
							IOflag = 0;	//reset the IO flag
							NoExecutions = 0;
							Current_Time--;	//Moving into ready queue, stop the counter from increasing causing a +1 error}
							
							//If we find a process that enters ready queue, undo all IO subtractions for the other processes
							for (int k = j-1; k >= 0; k--)
							{
								if (P[k].Process_State == 3)
								{
									P[k].IO[P[k].Current_IO]++;
								}
							}
							break;	//we found a process to go into the ready queue for execution so stop cycling the rest
						}
						//decrement current IO time for any process in IO
						else if (P[j].IO[P[j].Current_IO] > 0 && P[j].Process_State == 3)
						{
							P[j].IO[P[j].Current_IO]--;
						}
					}
					Current_Time++;	//increment time by 1
				}
				
				//*******************************************
				//Reprint completion for the idle context switch
				//********************************************
				if (completedFlag == 1)
				{
					cout << "Completed Processes:\n";
					out << "Completed Processes:\n";

					for (int x = 0; x < n; x++)
					{
						if (P[x].Process_State == 4)
						{
							cout << "     P" << x + 1 << "\n";
							out << "     P" << x + 1 << "\n";
						}
					}
					cout << "....................................................\n\n";
					out << "....................................................\n\n";
				}
			}
	}

	//***********************************************************
	//Program finished looping completely, print final state
	//***********************************************************
	cout << "Current Time: " << Current_Time << endl << endl;
	out << "Current Time: " << Current_Time << endl << endl;

	cout << "Now running: [idle]" << endl;
	cout << "......................................................\n\n";
	cout << "Ready Queue:  Process     Burst\n";
	out << "Now running: [idle]" << endl;
	out << "......................................................\n\n";
	out << "Ready Queue:  Process     Burst\n";

	Print(out);

	cout << "Now in I/O:  Process     Remaining I/O Time\n";
	out << "Now in I/O:  Process     Remaining I/O Time\n";

	cout << "             [Empty]                        \n\n";
	out << "             [Empty]                        \n\n";

	cout << "....................................................\n\n";
	out << "....................................................\n\n";

	cout << "Completed Processes:\n";
	out << "Completed Processes:\n";

	for (int x = 0; x < n; x++)
	{
		if (P[x].Process_State == 4)
		{
			cout << "     P" << x + 1 << "\n";
			out << "     P" << x + 1 << "\n";
		}
	}
	cout << "....................................................\n\n";
	out << "....................................................\n\n";
	

	//********************************
	//return CPU utilization variable
	//********************************
	return CPU_Utilization;
}


int main()
{
	const int n = 8;							//length of the array P[n]
	Process P[n] = { 1, 2, 3, 4, 5, 6, 7, 8 };	//processes 1 through 8 in an array P[n]
	Queue ReadyQ;								//the ready queue
	float AverageResponse, AverageTurnaround, AverageWait;	//variables for average response, wait, and turnaround
	float ResponseSum = 0, TurnaroundSum = 0, WaitSum = 0;	//summation of all 8 processes' wait, turnaround, and response for use with average
	int TotalTime = 0, CPU = 0;
	float CPU_Utilization = 0;

	ofstream out;			//variable to print to file
	out.open("FCFS.txt");	//open file

	if (out.fail())
	{
		cout << "Output file failed to open properly.\n";
		exit(1);
	}
	
	Initialize_Processes(P, n);					//initialize the processes using the function
	Initialize_Ready_Queue(ReadyQ, P, n);		//add the processes to the ready queue for the first time

	CPU = ReadyQ.Execution_FCFS(P, n, out);			//primary function FCFS execution, returns an integer for cpu calculation, assign to variabvle cpu

	for (int i = 0; i < n; i++)			//calculate the sums of response, turnaround, wait, and total time
	{
		ResponseSum = ResponseSum + P[i].Response_Time;
		TurnaroundSum = TurnaroundSum + P[i].Turnaround_Time;
		WaitSum = WaitSum + P[i].Wait_Time;
		TotalTime = max((P[i].Turnaround_Time + P[i].Arrival_Time), TotalTime);
	}

	CPU_Utilization = ((float)CPU / (float)TotalTime) * 100;	//calculate CPU utilization

	AverageResponse = ResponseSum / n;			//average response
	AverageTurnaround = TurnaroundSum / n;		//average turnaround
	AverageWait = WaitSum / n;			//average wait time

	//**********************************************************************************************
	//Print the information
	//*********************************************************************************************
	for (int j = 0; j < 8; j++)
	{
		cout << "The Response Time for Process " << j + 1 << " is: " << P[j].Response_Time << endl;
		cout << "The Wait Time for Process " << j + 1 << " is: " << P[j].Wait_Time << endl;
		cout << "The Turnaround Time for Process " << j + 1 << " is: " << P[j].Turnaround_Time << endl << endl;
		out << "The Response Time for Process " << j + 1 << " is: " << P[j].Response_Time << endl;
		out << "The Wait Time for Process " << j + 1 << " is: " << P[j].Wait_Time << endl;
		out << "The Turnaround Time for Process " << j + 1 << " is: " << P[j].Turnaround_Time << endl << endl;
	}

	cout << "The average Response Time for all processes is: " << AverageResponse << endl;
	cout << "The average Wait Time for all processes is: " << AverageWait << endl;
	cout << "The average Turnaround Time for all processes is: " << AverageTurnaround << endl << endl;
	out << "The average Response Time for all processes is: " << AverageResponse << endl;
	out << "The average Wait Time for all processes is: " << AverageWait << endl;
	out << "The average Turnaround Time for all processes is: " << AverageTurnaround << endl << endl;

	cout << "The total CPU utilization for all 8 processes is: " << CPU_Utilization << "%" << endl << endl;
	cout << "The total time required for all 8 processes is: " << TotalTime << endl << endl;
	out << "The total CPU utilization for all 8 processes is: " << CPU_Utilization << "%" << endl << endl;
	out << "The total time required for all 8 processes is: " << TotalTime << endl << endl;
	//*******************************************************************************************************

	out.close();		//close file used for writing to

	string dummyString;
	cout << "Type anything followed by the enter key to continue\n";
	cin >> dummyString;


	return 0;
}