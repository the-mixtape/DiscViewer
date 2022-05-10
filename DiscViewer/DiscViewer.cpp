#include <iostream>
#include <conio.h>

#include "DiscReader.h"


using namespace std;


#define ESC 27
#define ENTER 13
#define UP 72
#define DOWN 80


int Clamp(int Num, int Min, int Max)
{
	if(Num > Max)
	{
		Num = Min;
	}
	
	if(Num < Min)
	{
		Num = Max;
	}
	
	return Num;
}

void Run()
{
	DiscReader discReader;

	char c;

	int CurrentDrive = 0;
	while(true)
	{
		system("cls");
		discReader.ShowDiscsInfo();
		int CountDisc = discReader.GetDiscCount();

		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

		cout << "\n===================================================\n\n";

		for(int IndexDisc = 0; IndexDisc < CountDisc + 1; IndexDisc++)
		{

			if(CurrentDrive == IndexDisc)
			{
				SetConsoleTextAttribute(handle, 2);
			}
			
			cout << "disc " << IndexDisc << endl;

			SetConsoleTextAttribute(handle, 15);
		}
		
		c = _getch();

		if(c == ESC)
		{
			break;
		}
		
		if(c == ENTER)
		{
			system("cls");

			cout << "Disc " << CurrentDrive << " info: \n" << endl;
			
			discReader.ShowDiscInfo(CurrentDrive);

			cout << '\n';
			
			system("pause");
		}

		if(c == UP)
		{
			CurrentDrive = Clamp(--CurrentDrive, 0, CountDisc);
		}
		else if(c == DOWN)
		{
			CurrentDrive = Clamp(++CurrentDrive, 0, CountDisc);
		}
	}
	

}


int main()
{
	Run();
    
	return 0;
}
