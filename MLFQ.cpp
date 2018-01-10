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
	int QuueueLevel;	//1 = first queue Round Robin TQ = 6, 2 = second queue Round Robin TQ = 12, 3 = third queue FCFS
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
	int Execution_MLFQ(Process P[], int n, ofstream &out);
	void Print(ofstream &out);
private:
	qnode *front;
};

void Initialize_Processes(Process P[], int n);
void Initialize_Queue_1(Queue &Q, Process P[], int n);

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
	P[0] = { { 4, 5, 6, 7, 6, 4, 5, 4 },{ 15, 31, 26, 24, 41, 51, 16 }, 0, 0, 0, 1, 1, 0, 0, 0 };
	P[1] = { { 9, 11, 15, 12, 8, 11, 9, 10, 7 },{ 28, 22, 21, 28, 34, 34, 29, 31 }, 0, 0, 0, 1, 1, 0, 0, 0 };
	P[2] = { { 24, 12, 6, 17, 11, 22, 18 },{ 28, 21, 27, 21, 54, 31 }, 0, 0, 0, 1, 1, 0, 0, 0 };
	P[3] = { { 15, 14, 16, 18, 14, 13, 16, 15 },{ 35, 41, 45, 51, 61, 54, 61 }, 0, 0, 0, 1, 1, 0, 0, 0 };
	P[4] = { { 6, 5, 15, 4, 7, 4, 6, 10, 3 },{ 22, 21, 31, 26, 31, 18, 21, 33 }, 0, 0, 0, 1, 1, 0, 0, 0 };
	P[5] = { { 22, 27, 25, 11, 19, 18, 6, 6 },{ 38, 41, 29, 26, 32, 22, 26 }, 0, 0, 0, 1, 1, 0, 0, 0 };
	P[6] = { { 4, 7, 6, 5, 4, 7, 6, 5, 6, 9 },{ 36, 31, 32, 41, 42, 39, 33, 34, 21 }, 0, 0, 0, 1, 1, 0, 0, 0 };
	P[7] = { { 5, 4, 6, 4, 6, 5, 4, 6, 6 },{ 14, 33, 31, 31, 27, 21, 19, 11 }, 0, 0, 0, 1, 1, 0, 0, 0 };
}

//********************************************************
//Initalize Queue 1 with all processes in order (all arrival times 0)
//********************************************************
void Initialize_Queue_1(Queue &Q, Process P[], int n)
{
	for (int i = 0; i < 8; i++)
	{
		Q.Enqueue("               P" + to_string(i + 1) + "          " + to_string(P[i].CPU_Burst[0]) + "\n");
	}
}

int Queue::Execution_MLFQ(Process P[], int n, ofstream &out)
{
	Queue Queue2;				//Queue 2
	Queue Queue3;				//Queue 3
	qnode *p = front;			//pointer to front of a queue (queue 1 in this case)
	string executing = p->data;	//retrieve data in front and assign to string executing
	Dequeue();					//run dequeue to remove process to be executed from queue 1


	//***************************************************************
	//split the string data between process number and cpu burst
	//***************************************************************
	istringstream ss(executing);
	string token, processNumber;
	int Burst;
	int Current_Time = 0;
	int CPU_Utilization = 0;	//variable returned at the end of the function to calculate CPU utilization

	while (ss >> token)
	{
		if (!isdigit(token[0]))
		{
			processNumber = token[1];
		}
		stringstream(token) >> Burst;	//copy the burst value for process into variable Burst
	}

	//*********************************************************
	//convert process number to integer
	//*********************************************************
	int temp;
	stringstream(processNumber) >> temp;

	P[temp - 1].Process_State = 2;		//assign process state of current process to executing


	//************************************************************
	//Display basic output of Queue and IO information
	//*************************************************************
	cout << "Current Time: " << Current_Time << endl << endl;
	out << "Current Time: " << Current_Time << endl << endl;

	cout << "Now running: P" << processNumber << endl;
	cout << "......................................................\n\n";
	cout << "Queue 1:      Process     Burst\n";
	out << "Now running: P" << processNumber << endl;
	out << "......................................................\n\n";
	out << "Queue 1:      Process     Burst\n";

	Print(out);

	cout << "Queue 2:      Process     Burst\n";
	out << "Queue 2:      Process     Burst\n";

	Queue2.Print(out);

	cout << "Queue 3:      Process     Burst\n";
	out << "Queue 3:      Process     Burst\n";

	Queue3.Print(out);

	cout << "Now in I/O:  Process     Remaining I/O Time\n";
	cout << "             [Empty]                        \n\n";
	out << "Now in I/O:  Process     Remaining I/O Time\n";
	out << "             [Empty]                        \n\n";

	cout << "......................................................\n";
	cout << "......................................................\n\n";
	out << "......................................................\n";
	out << "......................................................\n\n";

	//**************************************************
	//Run the first CPU burst in RR TQ = 6 for process 1
	//**************************************************
	int TimeQuantum = 0;
	while (Burst > 0 && TimeQuantum < 6)
	{
		Current_Time++;
		//Increment wait time for all processes in ready state
		for (int j = 0; j < n; j++)
		{
			if (P[j].Process_State == 1)
			{
				P[j].Wait_Time++;
			}
		}
		Burst--;
		TimeQuantum++;
		CPU_Utilization++;
	}

	P[temp - 1].CPU_Burst[P[temp - 1].Current_Burst] = Burst;	//Change the actual CPU burst of process to the new Burst value

	//Check if CPU burst is 0 (in which case move to IO) or not (lower queue status)
	if (P[temp - 1].CPU_Burst[P[temp - 1].Current_Burst] == 0)
	{
		//Change current Process' CPU burst to the next burst
		P[temp - 1].Current_Burst++;
		//Move last executing (first process) process to IO
		P[temp - 1].Process_State = 3;
	}
	else
	{
		P[temp - 1].QuueueLevel = 2;	//reduce queue level to 2
		Queue2.Enqueue("               P" + to_string(temp) + "          " + to_string(P[temp-1].CPU_Burst[P[temp-1].Current_Burst]) + "\n");	//add to queue 2
		P[temp - 1].Process_State = 1;	//change to ready state from executing state
	}

	int completedFlag = 0;	//initialize completion flag for printing completions
	int AllCompletedFlag = 0;	//initialize ALL completion flag to end the MLFQ function
	int IOflag = 0;			//initialize IO flag for IO "queue"
	int ignoreOnce = 0;		//flag to ignore completion output the first time, explained later

	//run loop until all processes finish everything
	while (AllCompletedFlag != 1)
	{

		//Check to see if IO is empty for all processes, if so reset IO flag to 0
		if (P[0].Process_State != 3 && P[1].Process_State != 3 && P[2].Process_State != 3 && P[3].Process_State != 3
			&& P[4].Process_State != 3 && P[5].Process_State != 3 && P[6].Process_State != 3 && P[7].Process_State != 3)
		{
			IOflag = 0;
		}

		//******************************************************
		//Assign pointer p to null
		//*****************************************************
		qnode *p = 0;
		//If queue 1 has processes, assign pointer to front of queue 1
		if (!Empty())
		{
			p = front;
		}
		//else check queue 2, if processes exist, assign to front of queue 2
		else if (Empty() && !Queue2.Empty())
		{
			p = Queue2.front;
		}
		//else check queue 3 and point to front of queue 3
		else if (Empty() && Queue2.Empty() && !Queue3.Empty())
		{
			p = Queue3.front;
		}

		//*********************************************
		//extract the data from p
		//*********************************************
		string executing = p->data;

		istringstream ss(executing);
		string token, processNumber;

		//*****************************************************
		//Dequeue the appropriate queue that was extracted from
		//******************************************************
		if (!Empty())
		{
			Dequeue();
		}
		else if (Empty() && !Queue2.Empty())
		{
			Queue2.Dequeue();
		}
		else if (Empty() && Queue2.Empty() && !Queue3.Empty())
		{
			Queue3.Dequeue();
		}

		//******************************************************
		//Parse string into two subfields, process number and burst
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
		//code would need to be modified if arrival times were different
		if (P[temp - 1].Response_Time == 0 && (temp - 1) != 0)
		{
			P[temp - 1].Response_Time = Current_Time - P[temp - 1].Arrival_Time; // arrival time is 0 for all
		}


		//********************************************************
		//Display basic output for all three queues and IO
		//*******************************************************
		cout << "Current Time: " << Current_Time << endl << endl;
		out << "Current Time: " << Current_Time << endl << endl;

		cout << "Now running: P" << processNumber << endl;
		cout << "......................................................\n\n";
		cout << "Queue 1:     Process     Burst\n";
		out << "Now running: P" << processNumber << endl;
		out << "......................................................\n\n";
		out << "Queue 1:     Process     Burst\n";

		Print(out);

		cout << "Queue 2:      Process     Burst\n";
		out << "Queue 2:      Process     Burst\n";

		Queue2.Print(out);

		cout << "Queue 3:      Process     Burst\n";
		out << "Queue 3:      Process     Burst\n";

		Queue3.Print(out);

		cout << "Now in I/O:  Process     Remaining I/O Time\n";
		out << "Now in I/O:  Process     Remaining I/O Time\n";

		//********************************************************************
		//Check if IO has processes and print them if they exist
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
		//CPU BURST +  IO transition for ALL queues
		//*************************************	
		
		TimeQuantum = 0;
		//*****************************************************
		//If process is in queue 1, follow this code
		//******************************************************
		if (P[temp - 1].QuueueLevel == 1 && TimeQuantum == 0)
		{
			while (Burst > 0 && TimeQuantum < 6)	//run until either burst is 0 or time quantum expires
			{
				Current_Time++;		//increment time

				for (int j = 0; j < n; j++)		//loop through all other processes
				{
					//Increment wait time for all processes in ready state
					if (P[j].Process_State == 1)
					{
						P[j].Wait_Time++;
					}
					//if IO process is finished in IO, move from IO to ready state
					if (P[j].IO[P[j].Current_IO] == 0 && P[j].Process_State == 3)
					{
						P[j].Current_IO++;		//transition to the next IO in the array for future runs
						P[j].Process_State = 1;	//change to ready state

						//Check which queue it belongs in and assign it to the appropriate queue level
						if (P[j].QuueueLevel == 1)	
						{
							Enqueue("               P" + to_string(j + 1) + "          " + to_string(P[j].CPU_Burst[P[j].Current_Burst]) + "\n");
						}
						else if (P[j].QuueueLevel == 2)
						{
							Queue2.Enqueue("               P" + to_string(j + 1) + "          " + to_string(P[j].CPU_Burst[P[j].Current_Burst]) + "\n");
						}
						else if (P[j].QuueueLevel == 3)
						{
							Queue3.Enqueue("               P" + to_string(j + 1) + "          " + to_string(P[j].CPU_Burst[P[j].Current_Burst]) + "\n");
						}
						P[j].Wait_Time++;	//it's back in ready queue, need to add wait time
					}
					//decrement current IO time for any process still in IO
					else if (P[j].IO[P[j].Current_IO] > 0 && P[j].Process_State == 3)
					{
						P[j].IO[P[j].Current_IO]--;
					}
				}
				CPU_Utilization++;
				Burst--;		//reduce burst of currently executing process by 1
				TimeQuantum++;	//increase the time quantum by 1
			}

			//assign the new copied burst value to the actual CPU burst in the array for the executing process
			P[temp - 1].CPU_Burst[P[temp - 1].Current_Burst] = Burst;

			//Check if CPU burst is 0 (in which case move to IO) or not (lower queue status)
			if (P[temp - 1].CPU_Burst[P[temp - 1].Current_Burst] == 0)
			{
				//Change current Process' CPU burst to the next burst
				P[temp - 1].Current_Burst++;
				//Move last executing process to IO
				P[temp - 1].Process_State = 3;
			}
			else
			{
				P[temp - 1].QuueueLevel = 2;		//reduce queue state to queue 2
				Queue2.Enqueue("               P" + to_string(temp) + "          " + to_string(P[temp - 1].CPU_Burst[P[temp - 1].Current_Burst]) + "\n");	//add to queue 2
				P[temp - 1].Process_State = 1;	//change from executing to ready state
			}
		}
		//*************************************************************
		//If process is in queue 2, follow this code
		//*************************************************************
		else if (P[temp - 1].QuueueLevel == 2 && TimeQuantum == 0)
		{
			while (Burst > 0 && TimeQuantum < 12 && Empty())	//only run if queue 1 is empty, time quantum = 12
			{
				Current_Time++;		//increment time by 1

				for (int j = 0; j < n; j++)		//loop through all processes
				{
					//Increment wait time for all processes in ready state
					if (P[j].Process_State == 1)
					{
						P[j].Wait_Time++;
					}
					//Move IO Process into appropriate queue if IO time expires
					if (P[j].IO[P[j].Current_IO] == 0 && P[j].Process_State == 3)
					{
						P[j].Current_IO++;		//transition to the next IO in the array for future runs
						P[j].Process_State = 1;		//change to ready state

						//Check which queue level it belongs to
						//if queue 1
						if (P[j].QuueueLevel == 1)
						{
							Enqueue("               P" + to_string(j + 1) + "          " + to_string(P[j].CPU_Burst[P[j].Current_Burst]) + "\n");		//add to queue 1
							Current_Time--;		//we need to preempt this process with queue 1 so remove the time we added earlier
							Burst++;			//need to preempt this process with queue 1 so add back the burst that will be subtracted
							CPU_Utilization--;
							for (int k = j - 1; k >= 0; k--)	//loop through all of the processes in reverse order
							{
								if (P[k].Process_State == 3)		//if the process was in IO state
								{
									P[k].IO[P[k].Current_IO]++;		//add back the IO time because we need to immediately run queue 1, this run doesn't count
								}
								if (P[k].Process_State == 1)		//if process was in the ready state
								{
									P[k].Wait_Time--;			//remove the wait time we just added because we need to preempt
								}
							}
							break;				//exit this loop, do not search other processes, do not continue executing this queue
						}
						//if queue 2
						else if (P[j].QuueueLevel == 2)
						{
							Queue2.Enqueue("               P" + to_string(j + 1) + "          " + to_string(P[j].CPU_Burst[P[j].Current_Burst]) + "\n");	//add to queue 2
							P[j].Wait_Time++;	//it's back in ready queue, need to add wait time
						}
						else if (P[j].QuueueLevel == 3)
						{
							Queue3.Enqueue("               P" + to_string(j + 1) + "          " + to_string(P[j].CPU_Burst[P[j].Current_Burst]) + "\n");	//add to queue 3
							P[j].Wait_Time++;	//it's back in ready queue, need to add wait time
						}
					}
					//decrement current IO time for any process in IO
					else if (P[j].IO[P[j].Current_IO] > 0 && P[j].Process_State == 3)
					{
						P[j].IO[P[j].Current_IO]--;
					}
				}
				CPU_Utilization++;
				Burst--;		//reduce burst by 1
				TimeQuantum++;	//increment time quantum by 1
			}

			//assign the new copied burst value to the actual CPU burst in the array for the executing process
			P[temp - 1].CPU_Burst[P[temp - 1].Current_Burst] = Burst;

			//Check if CPU burst is 0 (in which case move to IO)
			if (P[temp - 1].CPU_Burst[P[temp - 1].Current_Burst] == 0)
			{
				//Change current Process' CPU burst to the next burst
				P[temp - 1].Current_Burst++;
				//Move last executing process to IO
				P[temp - 1].Process_State = 3;
			}
			//if burst is not 0 and time quantum did NOT finish, this queue was preempted, keep in same queue, add back to ready
			else if (P[temp - 1].CPU_Burst[P[temp - 1].Current_Burst] != 0 && TimeQuantum < 12)
			{
				Queue2.Enqueue("               P" + to_string(temp) + "          " + to_string(P[temp - 1].CPU_Burst[P[temp - 1].Current_Burst]) + "\n");	//add back to queue 2
				P[temp - 1].Process_State = 1;				//change state to ready
			}
			//if burst is not 0 and time quantum DID finish, this queue needs to be lowered to 3
			else if (P[temp - 1].CPU_Burst[P[temp - 1].Current_Burst] != 0 && TimeQuantum >= 12)
			{
				P[temp - 1].QuueueLevel = 3;	//change queue state to 3
				Queue3.Enqueue("               P" + to_string(temp) + "          " + to_string(P[temp - 1].CPU_Burst[P[temp - 1].Current_Burst]) + "\n");	//add to queue 3
				P[temp - 1].Process_State = 1;	//add to ready state
			}
		}
		//************************************************************
		//If process is in queue 3, run this code
		//************************************************************
		else if (P[temp - 1].QuueueLevel == 3 && TimeQuantum == 0)
		{
			while (Empty() && Queue2.Empty() && Burst > 0)		//only run this code if queue 1 and 2 are both empty, FCFS
			{
				Current_Time++;		//increment time by 1

					for (int j = 0; j < n; j++)		//loop through processes
					{
						//Increment wait time for all processes in ready state
						if (P[j].Process_State == 1)
						{
							P[j].Wait_Time++;
						}
						//Move IO Process into appropriate queue if IO time expires
						if (P[j].IO[P[j].Current_IO] == 0 && P[j].Process_State == 3)
						{
							P[j].Current_IO++;		//transition to the next IO in the array for future runs
							P[j].Process_State = 1;	//change state to ready

							//Check which queue the process belongs to
							//For queue 1...
							if (P[j].QuueueLevel == 1)
							{
								Current_Time--;		//need to preempt current executing process with this queue. Remove time we just added.
								Burst++;			//need to preempt current executing process with this queue. Add back the burst that will be subtracted. 
								CPU_Utilization--;
								Enqueue("               P" + to_string(j + 1) + "          " + to_string(P[j].CPU_Burst[P[j].Current_Burst]) + "\n");	//add this process to queue 1
								for (int k = j - 1; k >= 0; k--)	//loop through all processes in reverse order
								{
									if (P[k].Process_State == 3)		//if process is in IO
									{
										P[k].IO[P[k].Current_IO]++;		//add back the IO time we removed, need to preempt
									}
									if (P[k].Process_State == 1)	//if process is in ready state
									{
										P[k].Wait_Time--;		//remove wait time we just added, need to preempt. This iteration "never happened"
									}
								}
								break;		//exit this loop, do not search other processes, do not continue executing this queue
							}
							//if process leaving IO is in queue 2
							else if (P[j].QuueueLevel == 2)
							{
								Current_Time--;				//need to preempt current executing process with this queue. Remove time we just added.
								Burst++;					//need to preempt current executing process with this queue. Add back the burst that will be subtracted.
								CPU_Utilization--;
								Queue2.Enqueue("               P" + to_string(j + 1) + "          " + to_string(P[j].CPU_Burst[P[j].Current_Burst]) + "\n"); //add this process to queue 2
								for (int k = j - 1; k >= 0; k--)	//loop through all processes in reverse order
								{
									if (P[k].Process_State == 3)	//if process is in IO
									{
										P[k].IO[P[k].Current_IO]++;		//add back the IO time we removed, need to preempt
									}
									if (P[k].Process_State == 1)	//if process is in ready state
									{
										P[k].Wait_Time--;		//remove wait time we just added, need to preempt. This iteration "never happened"
									}
								}
								break;	//exit this loop, do not search other processes, do not continue executing this queue
							}
							//process leaving IO is in queue 3
							else if (P[j].QuueueLevel == 3)
							{
								Queue3.Enqueue("               P" + to_string(j + 1) + "          " + to_string(P[j].CPU_Burst[P[j].Current_Burst]) + "\n");	//add back to queue 3
								P[j].Wait_Time++;	//it's back in ready queue, need to add wait time
							}
						}
						//decrement current IO time for any process in IO
						else if (P[j].IO[P[j].Current_IO] > 0 && P[j].Process_State == 3)
						{
							P[j].IO[P[j].Current_IO]--;
						}
					}
					CPU_Utilization++;
					Burst--;	//reduce burst by 1. This is first come first serve so no need for time quantum
				}

			//assign the new copied burst value to the actual CPU burst in the array for the executing process
			P[temp - 1].CPU_Burst[P[temp - 1].Current_Burst] = Burst;

			//Check if CPU burst is 0 (in which case move to IO) otherwise, add to the back of queue 3. Only three queues so it can't fall any lower
			if (P[temp - 1].CPU_Burst[P[temp - 1].Current_Burst] == 0)
			{
				//Change current Process' CPU burst to the next burst
				P[temp - 1].Current_Burst++;
				//Move last executing (first process) process to IO
				P[temp - 1].Process_State = 3;
			}
			else
			{
				Queue3.Enqueue("               P" + to_string(temp) + "          " + to_string(P[temp - 1].CPU_Burst[P[temp - 1].Current_Burst]) + "\n");	//Add to queue 3
				P[temp - 1].Process_State = 1;	//process state is ready
			}
		}

		//Check if previously executed process has no more CPU burst left, mark completed if so
		if (P[temp - 1].CPU_Burst[P[temp - 1].Current_Burst + 1] == 0 && P[temp - 1].CPU_Burst[P[temp - 1].Current_Burst] == 0 && P[temp - 1].Process_State != 4)
		{
			P[temp - 1].Turnaround_Time = Current_Time - P[temp - 1].Arrival_Time;	//arrival time is 0 so turnaround time is just the time it finishes
			P[temp - 1].Process_State = 4;	//state is completed
			if (completedFlag == 0) //if complete flag hasn't been triggered yet, trigger it, otherwise skip code (redundant)
			{
				completedFlag = 1;		//flag to mark completion
				ignoreOnce = 1;			//when first process completes, don't want to print until next time unit. Flag to skip printing first completion
			}
		}


		//Check if all processes have finished completing. Code must be here to avoid infinitely looping in next section if all queues are empty and IO is empty
		if (P[0].Process_State == 4 && P[1].Process_State == 4 && P[2].Process_State == 4 && P[3].Process_State == 4 &&
			P[4].Process_State == 4 && P[5].Process_State == 4 && P[6].Process_State == 4 && P[7].Process_State == 4)
		{
			AllCompletedFlag = 1;
		}

		//**************************************************
		//Check if there are completed processes and if so, print them
		//***************************************************
		if (completedFlag == 1 && ignoreOnce == 0)
		{
			cout << "Completed Processes:\n";
			out << "Completed Processes:\n";

			for (int x = 0; x < n; x++)
			{
				if (P[x].Process_State == 4 && x != (temp - 1))
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
		//Check if all three queues are empty but there are still processes in the IO
		//***********************************************************************
		while (Empty() && Queue2.Empty() && Queue3.Empty() && AllCompletedFlag != 1)
		{

			cout << "Current Time: " << Current_Time << endl << endl;
			out << "Current Time: " << Current_Time << endl << endl;

			cout << "Now running: [idle]" << endl;
			cout << "......................................................\n\n";
			cout << "Queue 1:     Process     Burst\n";
			out << "Now running: [idle]" << endl;
			out << "......................................................\n\n";
			out << "Queue 1:     Process     Burst\n";

			Print(out);

			cout << "Queue 2:      Process     Burst\n";
			out << "Queue 2:      Process     Burst\n";

			Queue2.Print(out);

			cout << "Queue 3:      Process     Burst\n";
			out << "Queue 3:      Process     Burst\n";

			Queue3.Print(out);

			cout << "Now in I/O:  Process     Remaining I/O Time\n";
			out << "Now in I/O:  Process     Remaining I/O Time\n";

			//********************************************************************
			//Check if IO has processes and print them if they exist
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
			cout << "......................................................\n";
			cout << "......................................................\n\n";
			out << "......................................................\n";
			out << "......................................................\n\n";

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

			//if ALL queues are empty, no processes are executing, and IO still has processes then...
			while (Empty() && NoExecutions == 1 && Queue2.Empty() && Queue3.Empty() && AllCompletedFlag != 1)
			{
				for (int j = 0; j < n; j++)		//loop through all processes
				{
					//Move Process into ready queue once it reaches expiration
					if (P[j].IO[P[j].Current_IO] == 0 && P[j].Process_State == 3)
					{
						P[j].Current_IO++;		//transition to the next IO in the array for future runs
						P[j].Process_State = 1;	//change to ready state
						//Find which queue the process belongs to
						if (P[j].QuueueLevel == 1)
						{
							Enqueue("               P" + to_string(j + 1) + "          " + to_string(P[j].CPU_Burst[P[j].Current_Burst]) + "\n");	//add to queue 1
						}
						else if (P[j].QuueueLevel == 2)
						{
							Queue2.Enqueue("               P" + to_string(j + 1) + "          " + to_string(P[j].CPU_Burst[P[j].Current_Burst]) + "\n");	//add to queue 2
						}
						else if (P[j].QuueueLevel == 3)
						{
							Queue3.Enqueue("               P" + to_string(j + 1) + "          " + to_string(P[j].CPU_Burst[P[j].Current_Burst]) + "\n");	//add to queue 3
						}
						IOflag = 0;	//reset the IO flag
						NoExecutions = 0;	//reset no execution flag
						Current_Time--;	//Moving into a ready queue, stop the counter from increasing causing a +1 error
										
						//If we find a process that enters ready queue, undo all IO subtractions for the other processes
						for (int k = j - 1; k >= 0; k--)
						{
							if (P[k].Process_State == 3)
							{
								P[k].IO[P[k].Current_IO]++;
							}
						}
						break;	//we found a process to go into the ready queue for execution so stop cycling the rest
					}
					//otherwise decrement current IO time for any process in IO
					else if (P[j].IO[P[j].Current_IO] > 0 && P[j].Process_State == 3)
					{
						P[j].IO[P[j].Current_IO]--;
					}
				}
				Current_Time++;	//increment time by 1
			}

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

		//Check the IO status of all processes again to check if a process finished at the exact same time that the last executing process finished
		for (int j = 0; j < n; j++)
		{
			//Move IO Process into ready queue if IO time expires
			if (P[j].IO[P[j].Current_IO] == 0 && P[j].Process_State == 3)
			{
				//check which queue the process belongs to
				if (P[j].QuueueLevel == 1)
				{
					P[j].Current_IO++;		//transition to the next IO in the array for future runs
					Enqueue("               P" + to_string(j + 1) + "          " + to_string(P[j].CPU_Burst[P[j].Current_Burst]) + "\n");	//add to queue 1
				}
				else if (P[j].QuueueLevel == 2)
				{
					P[j].Current_IO++;		//transition to the next IO in the array for future runs
					Queue2.Enqueue("               P" + to_string(j + 1) + "          " + to_string(P[j].CPU_Burst[P[j].Current_Burst]) + "\n");	//add to queue 2
				}
				else if (P[j].QuueueLevel == 3)
				{
					P[j].Current_IO++;		//transition to the next IO in the array for future runs
					Queue3.Enqueue("               P" + to_string(j + 1) + "          " + to_string(P[j].CPU_Burst[P[j].Current_Burst]) + "\n");	//add to queue 3
				}
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
	cout << "Queue 1:     Process     Burst\n";

	out << "Now running: [idle]" << endl;
	out << "......................................................\n\n";
	out << "Queue 1:     Process     Burst\n";

	Print(out);

	cout << "Queue 2:      Process     Burst\n";
	out << "Queue 2:      Process     Burst\n";

	Queue2.Print(out);

	cout << "Queue 3:      Process     Burst\n";
	out << "Queue 3:      Process     Burst\n";

	Queue3.Print(out);

	cout << "Now in I/O:  Process     Remaining I/O Time\n";
	out << "Now in I/O:  Process     Remaining I/O Time\n";

	cout << "             [Empty]                        \n\n";
	out << "             [Empty]                        \n\n";

	cout << "......................................................\n\n";
	out << "......................................................\n\n";

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

	return CPU_Utilization;
}


int main()
{
	const int n = 8;
	Process P[n] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	Queue Queue1; 
	float AverageResponse, AverageTurnaround, AverageWait;
	float ResponseSum = 0, TurnaroundSum = 0, WaitSum = 0;
	int TotalTime = 0, CPU = 0;
	float CPU_Utilization = 0;

	ofstream out;

	out.open("MLFQ.txt");

	if (out.fail())
	{
		cout << "Output file failed to open properly.\n";
		exit(1);
	}

	Initialize_Processes(P, n);
	Initialize_Queue_1(Queue1, P, n);

	CPU = Queue1.Execution_MLFQ(P, n, out);

	for (int i = 0; i < n; i++)
	{
		ResponseSum = ResponseSum + P[i].Response_Time;
		TurnaroundSum = TurnaroundSum + P[i].Turnaround_Time;
		WaitSum = WaitSum + P[i].Wait_Time;
		TotalTime = max((P[i].Turnaround_Time + P[i].Arrival_Time), TotalTime);
	}

	CPU_Utilization = ((float)CPU /(float) TotalTime) * 100;

	AverageResponse = ResponseSum / n;
	AverageTurnaround = TurnaroundSum / n;
	AverageWait = WaitSum / n;

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

	out.close();

	string dummyString;
	cout << "Type anything followed by the enter key to continue\n";
	cin >> dummyString;


	return 0;
}