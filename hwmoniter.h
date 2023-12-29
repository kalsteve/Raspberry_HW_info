#ifndef _HWMONITOR_H_
#define _HWMONITOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>

#define MAX(a,b) (((a)>(b))?(a):(b))

typedef struct board_info_
{
    char *model;
    char *hardware;
    char *version;
    char *serial_number;
    char *linux_version;
}board_info;

typedef struct Cpu_data_
{
    unsigned long long int cpu_total_time;
    unsigned long long int cpu_user_time;
    unsigned long long int cpu_nice_time;
    unsigned long long int cpu_system_time;
    unsigned long long int cpu_idle_time;
    unsigned long long int cpu_iowait_time;
    unsigned long long int cpu_irq_time;
    unsigned long long int cpu_softirq_time;
    unsigned long long int cpu_steal_time;
    unsigned long long int cpu_guest_time;
    unsigned long long int cpu_guest_nice;

    unsigned long long int cpu_total_time_period;
    unsigned long long int cpu_user_time_period;
    unsigned long long int cpu_nice_time_period;
    unsigned long long int cpu_system_time_period;
    unsigned long long int cpu_idle_time_period;
    unsigned long long int cpu_iowait_time_period;
    unsigned long long int cpu_irq_time_period;
    unsigned long long int cpu_softirq_time_period;
    unsigned long long int cpu_steal_time_period;
    unsigned long long int cpu_guest_time_period;
    unsigned long long int cpu_guest_nice_period;

}Cpu_data;


typedef struct Cpu_info_
{
    Cpu_data cpu_data;
    double usage;
    unsigned long long int clock;
    double temp;
}Cpu_info;

typedef struct Ram_info_
{
    double usage;
    unsigned long long int total;
    unsigned long long int used;
    unsigned long long int available;
    unsigned long long int free;
}Ram_info;

typedef struct Disk_info_
{
    char *name;
    double usage;
    unsigned long long int total_size;
    unsigned long long int used_size;
    unsigned long long int free_size;
}Disk_info;

typedef struct Network_info
{
    unsigned long int receive_bytes;
    unsigned long long int receive_bytes_period;

    unsigned long long int transmit_bytes;
    unsigned long long int transmit_bytes_period;

    unsigned long long int receive_per_second;
    unsigned long long int receive_per_second_period;
    unsigned long long int transmit_per_second;
    unsigned long long int transmit_per_second_period;

    double receive_percent;
    double transmit_percent;
}Network_info;

typedef struct Hw_info_ 
{
    int cpu_count;
    int disk_count;
    board_info board_info;
    Cpu_info *cpu_info;
    Ram_info ram_info;
    Disk_info *disk_info;
    Network_info network_info;
}Hw_info;

Hw_info* Hw_info_new();
int Hw_info_delete(Hw_info *);
int initDiskinfo(Hw_info *);
int initCpuinfo(Hw_info *);
int getBoardInfo(Hw_info *);
int getDiskInfo(Hw_info *);
int getNetworkInfo(Hw_info *);
int getRamInfo(Hw_info *);
int getCpusInfo(Hw_info *);

#endif