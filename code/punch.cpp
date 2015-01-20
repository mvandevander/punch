/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//  PUNCH.CPP                                                              //
//                                                                         //
//  A simple Windows command-line utility for keeping track of time spent  //
//  on personal projects.                                                  //
//                                                                         //
//  Copyright 2015: John Matthew VanDevander                               //
//                                                                         //
//  You are welcome to modify this code for your personal use. I would     //
//  definitely appreciate it if you notified me if you make any significant//
//  improvements. :)                                                       //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <windows.h>

int main(int argc, char *argv[])
{
    bool i_am_just_reading = false;
    while(--argc > 0)
    {
        char* arg = argv[argc];
        if(!strcmp(arg,"-clear") || !strcmp(arg,"/clear") ||
           !strcmp(arg,"-c")     || !strcmp(arg,"/c"))
        {
            DeleteFile("punch.time");
            printf("Time clock cleared.\n");
            return 0;
        }
        else if(!strcmp(arg,"-read") || !strcmp(arg,"/read") ||
                !strcmp(arg,"-r")    || !strcmp(arg,"/r"))
        {
            i_am_just_reading = true;
        }
        else if(!strcmp(arg,"-help") || !strcmp(arg,"-?") ||
                !strcmp(arg,"/help") || !strcmp(arg,"/?"))
        {
            printf("A virtual time-clock for tracking time worked on personal projects.\n");
            printf("\nPUNCH [/clear | /c] [/read | /r]\n");
            printf("\n");
            printf("    /c:     Clears the total worked time to zero.\n");
            printf("    /r:     Shows (but doesn't modify) the current time and if you\n");
            printf("            are clocked in.\n");
            return 0;
            
        }
        else
        {
            printf("I'm sorry, I didn't understand that command, try 'punch /help'\n");
            return 0;
        }
    }
    SYSTEMTIME system_time;
    GetSystemTime(&system_time);
    FILETIME file_time;
    SystemTimeToFileTime(&system_time, &file_time);
    ULARGE_INTEGER file_time_integer;
    file_time_integer.LowPart = file_time.dwLowDateTime;
    file_time_integer.HighPart = file_time.dwHighDateTime;
    FILE* file;
    ULARGE_INTEGER saved_last_file_time_integer = file_time_integer;
    ULARGE_INTEGER saved_total_file_time_integer = {};
    bool i_am_currently_punched_in = false;
    bool file_open_failed = fopen_s(&file, "punch.time","rb");
    if(file_open_failed == false)
    {
        void* memory = malloc(1);
        fread(memory, 1, 1, file);
        char saved_punch_status = *(char *)memory;
        if(saved_punch_status > 0)
        {
            i_am_currently_punched_in = true;
        }
        free(memory);
        memory = malloc(sizeof(ULARGE_INTEGER));
        fread(memory, sizeof(ULARGE_INTEGER), 1, file);
        saved_last_file_time_integer = *(ULARGE_INTEGER *)memory;
        fread(memory, sizeof(ULARGE_INTEGER), 1, file);
        saved_total_file_time_integer = *(ULARGE_INTEGER *)memory;
        free(memory);
        fclose(file);
    }
    char punch_status = 1;
    ULARGE_INTEGER file_time_diff = {};
    file_time_diff.QuadPart = file_time_integer.QuadPart - saved_last_file_time_integer.QuadPart;
    ULARGE_INTEGER total_file_time_integer;
    total_file_time_integer.QuadPart = saved_total_file_time_integer.QuadPart;
    if(i_am_currently_punched_in)
    {
        total_file_time_integer.QuadPart += file_time_diff.QuadPart;
    }
    if(i_am_just_reading == false)
    {
        file_open_failed = fopen_s(&file, "punch.time","wb");
        if(file_open_failed)
        {
            printf("ERROR: Failed to open file: 'punch.time' for writing.\n");
            printf("       Is the file read-only or hidden?\n\n");
        }
        else
        {
            if(i_am_currently_punched_in)
            {
                printf("\n---PUNCHING OUT---\n\n");
                i_am_currently_punched_in = false;
                punch_status = 0;
            }
            else
            {
                printf("\n---PUNCHING IN---\n\n");
                i_am_currently_punched_in = true;
            }
            fwrite((void*)&punch_status,1,1,file);
            fwrite((void*)&file_time_integer,sizeof(ULARGE_INTEGER),1,file);
            fwrite((void*)&total_file_time_integer,sizeof(ULARGE_INTEGER),1,file);
            fclose(file);
        }
        
    }
    
    if(i_am_currently_punched_in)
    {
        printf("You are currently ON the clock\n");
    }
    else
    {
        printf("You are currently OFF the clock\n");
    }
    
    ULARGE_INTEGER TotalSeconds = {};
    TotalSeconds.QuadPart = (unsigned long long)total_file_time_integer.QuadPart/(unsigned long long)10000000;
    bool printing_time_worked = (TotalSeconds.QuadPart != 0);
    if(printing_time_worked)
    {
        printf("(Total Time Worked: ");
        unsigned int Seconds = TotalSeconds.QuadPart % 60; 
        TotalSeconds.QuadPart /= (unsigned long long)60;
        unsigned int Minutes = TotalSeconds.QuadPart % 60;
        TotalSeconds.QuadPart /= (unsigned long long)60;
        unsigned int Hours = TotalSeconds.QuadPart % 24;
        TotalSeconds.QuadPart /= (unsigned long long)24;
        unsigned int Days = TotalSeconds.QuadPart % 365;
        TotalSeconds.QuadPart /= (unsigned long long)365;
        unsigned int NonLeapYears = (unsigned int)TotalSeconds.QuadPart;
        
        if(NonLeapYears)
        {
            printf("%iy", NonLeapYears);
        }
        if(Days)
        {
            printf("%id", Days);
        }
        if(Hours)
        {
            printf("%ih", Hours);
        }
        if(Minutes)
        {
            printf("%im", Minutes);
        }
        if(Seconds)
        {
            printf("%is", Seconds);
        }
        
        printf(")\n");
    }    
    return 0;
}