/*
hwmonitor.c  
라즈베리파이 하드웨어 모니터링
*/
#include "hwmoniter.h"

const int CPU_MAX_COUNT = 8;
const int DISKS_MAX = 8;
const int BUFFSIZE = 128;
const char *BOARD = "/proc/cpuinfo";
const char *VERSION = "/proc/version";
const char *STAT = "/proc/stat";
const char *CPU = "/sys/devices/system/cpu/cpu";
const char *CPU_CLOCK = "/cpufreq/scaling_cur_freq";
const char *DISK = "/proc/diskstats";
const char *DISKS = "/proc/mounts";
const char *DISK_FIND = "/dev/sd";
const char *DISK_PATH = "/dev/";
const char *RAM = "/proc/meminfo";
const char *NETWORK = "/proc/net/dev";
const char *CPU_TEMP = "/sys/class/thermal/thermal_zone0/temp";

//하드웨어 정보를 생성하는 함수
Hw_info* Hw_info_new()
{
    Hw_info* this;

    //메모리 할당
    this = calloc(1, sizeof(Hw_info));

    //cpuinfo 초기화
    if(initCpuinfo(this) == -1)
    {
        printf("Error: initCpuinfo error\n");
        return NULL;
    }

    //diskinfo 초기화
    if(initDiskinfo(this) == -1)
    {
        printf("Error: initDiskinfo error\n");
        return NULL;
    }

    //보드 정보 초기화
    if(getBoardInfo(this) == -1)
    {
        printf("Error: getBoardinfo error\n");
        return NULL;
    }

    //cpu 정보 초기화
    if(getCpusInfo(this) == -1)
    {
        printf("Error: getCpuinfo error\n");
        return NULL;
    }

    //ram 정보 초기화
    if(getRamInfo(this) == -1)
    {
        printf("Error: getRaminfo error\n");
        return NULL;
    }

    //disk 정보 초기화
    if(getDiskInfo(this) == -1)
    {
        printf("Error: getDiskInfo error\n");
        return NULL;
    }

    //network 정보 초기화
    if(getNetworkInfo(this) == -1)
    {
        printf("Error: getNetworkinfo error\n");
        return NULL;
    }

    return this;
}

//cpuinfo를 초기화하는 함수
int initCpuinfo(Hw_info *this)
{
    FILE *file_pointer;
    char buffer[BUFFSIZE];
    int cpu_count = 0;
    int counter = 0;

    //메모리 점유 여부 확인
    if(this == NULL)
    {
        printf("Error: Hw_info is NULL\n");
        return -1;
    }

    //cpuinfo 파일 오픈
    file_pointer = fopen(STAT, "r");

    //파일 오픈 여부 확인
    if (file_pointer == NULL)
    {
        printf("Error: %s file open error\n", STAT);
        return -1;
    }

    //cpu 갯수 확인
    while(fgets(buffer, BUFFSIZE - 1, file_pointer))
    {
        
        if(strncmp(buffer, "cpu", 3) == 0)
        {
            cpu_count++;
        }
        
    }
    
    //cpu 갯수 저장
    this->cpu_count = MAX(cpu_count - 1, 1);
    
    //파일 포인터 닫기
    fclose(file_pointer);

    cpu_count++;
    //cpu_info cpu 갯수에 맞게 메모리 할당
    this->cpu_info = (Cpu_info *)calloc(cpu_count, sizeof(Cpu_info));

    //cpu_info 초기화
    for(counter = 0 ; counter < cpu_count ; counter++)
    {
        this->cpu_info[counter].cpu_data.cpu_total_time = 1;
        this->cpu_info[counter].cpu_data.cpu_total_time_period = 1;
    }
}

int initDiskinfo(Hw_info *this)
{
    FILE *file_pointer;
    char buffer[BUFFSIZE];
    int disk_count = 0;
    //문자열을 넣을 포인터 배열 동적 할당
    char **str = (char *)calloc(DISKS_MAX, sizeof(char*));

    //메모리 점유 여부 확인
    if(this == NULL)
    {
        printf("Error: Hw_info is NULL\n");
        return -1;
    }

    file_pointer = fopen(DISK, "r");

    if(file_pointer == NULL)
    {
        printf("Error: %s file open error\n", DISK);
        return -1;
    }

    while(fgets(buffer, BUFFSIZE - 1, file_pointer))
    {
        if(strchr(buffer, 's') != NULL)
        {
            char str_buffer[BUFFSIZE];
            char *temp;
            sscanf(buffer, "%*d %*d %s", str_buffer);
            if(strlen(str_buffer) > 3)
            {
                temp = (char *)calloc(strlen(str_buffer)+1, sizeof(char));
                strcpy(temp, str_buffer);
                str[disk_count] = temp;
                disk_count++;
            }
            
        }
    }

    fclose(file_pointer);
    
    this->disk_count = disk_count;
    this->disk_info = calloc(disk_count, sizeof(Disk_info));

    for(int counter = 0 ; counter < disk_count ; counter++)
    {
        this->disk_info[counter].name = str[counter];
    }

    return 1;
}

int Hw_info_delete(Hw_info *this)
{
    free(this->cpu_info);
    free(this->disk_info);
    free(this);
    return 0;
}

int getDiskInfo(Hw_info *this)
{
    FILE *file_pointer;
    int counter = 0 ;
    char buffer[BUFFSIZE];
    file_pointer = fopen(DISKS, "r");
    if(file_pointer == NULL)
    {
        printf("Error: %s file open error\n", DISKS);
        return -1;
    }

    while(fgets(buffer,BUFFSIZE-1,file_pointer))
    {
        if(strncmp(buffer, DISK_FIND, strlen(DISK_FIND)) == 0)
        {
            for(int i = counter ; i < this->disk_count ; i++)
            {
                char temp[BUFFSIZE];
                memset(temp, 0, BUFFSIZE);
                sprintf(temp, "%s%s", DISK_PATH, this->disk_info[counter].name);
                if(strncmp(buffer, temp, strlen(temp)) == 0)
                {
                    counter++;
                    memset(temp, 0, BUFFSIZE);
                    sscanf(buffer,"%*s %s", temp);

                    struct statvfs stat;
                    if(statvfs(temp, &stat) == -1)
                    {
                        printf("Error: statvfs error\n");
                        return -1;
                    }

                    this->disk_info[i].total_size = (unsigned long long)stat.f_blocks * stat.f_bsize / 1024;
                    this->disk_info[i].free_size = (unsigned long long)stat.f_bavail * stat.f_bsize / 1024;
                    this->disk_info[i].used_size = (unsigned long long)this->disk_info[i].total_size - this->disk_info[i].free_size;
                    this->disk_info[i].usage = (double)this->disk_info[i].used_size / (double)this->disk_info[i].total_size * 100.0;

                }
            }
            
            
        }
    }

}

int getBoardInfo(Hw_info *this)
{
    FILE *file_pointer;
    char buffer[1024];
    char *str;
    file_pointer = fopen(BOARD, "r");

    if(file_pointer == NULL)
    {
        printf("Error: /proc/cpuinfo file open error\n");
        return -1;
    }

    while(fgets(buffer, BUFFSIZE - 1, file_pointer))
    {
        if(strncmp(buffer, "Model", 5) == 0)
        {
            char temp[BUFFSIZE];
            sscanf(buffer, "%*s : %s", temp);
            str = calloc(strlen(temp)+1, sizeof(char));
            strcpy(str, temp);
            this->board_info.model = str;
            
        }
        else if(strncmp(buffer, "Hardware", 8) == 0)
        {
            char temp[BUFFSIZE];
            sscanf(buffer, "%*s : %s", temp);
            str = calloc(strlen(temp)+1, sizeof(char));
            strcpy(str, temp);
            this->board_info.hardware = str;
        }
        else if(strncmp(buffer, "Revision", 8) == 0)
        {
            char temp[BUFFSIZE];
            sscanf(buffer, "%*s : %s", temp);
            str = calloc(strlen(temp)+1, sizeof(char));
            strcpy(str, temp);
            this->board_info.version = str;
        }
        else if(strncmp(buffer, "Serial", 6) == 0)
        {
            char temp[BUFFSIZE];
            sscanf(buffer, "%*s : %s", temp);
            str = calloc(strlen(temp)+1, sizeof(char));
            strcpy(str, temp);
            this->board_info.serial_number = str;
        }
    }

    fclose(file_pointer);
    
    file_pointer = fopen(VERSION, "r");

    if(file_pointer == NULL)
    {
        printf("Error: /proc/version file open error\n");
        return -1;
    }

    char temp[1024];
    fgets(buffer, 1024 - 1, file_pointer);
    strcpy(temp, strtok(buffer, "\n"));
    this->board_info.linux_version = temp;
    fclose(file_pointer);

    return 1;
}

//cpu 정보를 가져오는 함수
int getCpusInfo(Hw_info *this)
{
    char buffer[BUFFSIZE];
    int counter = 0;
    int cpu_count = this->cpu_count;
    Cpu_info *cpu_info = this->cpu_info;
    FILE *file_pointer;

    file_pointer = fopen(STAT, "r");

    if(file_pointer == NULL)
    {
        printf("Error: CPU file open error\n");
        return -1;
    }

    for(int i = 0 ; i <= cpu_count ; i++)
    {
        char *error;
        unsigned long long int diff_total_time;
        unsigned long long int diff_idle_time;
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
        error = fgets(buffer, BUFFSIZE - 1, file_pointer);

        if(error == NULL)
        {
            printf("Error: CPU file read error\n");
            return -1;
        }

        if(strncmp(buffer, "cpu", 3) == 0)
        {
            sscanf(
            buffer, 
            "%*s %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu",
            &cpu_user_time,
            &cpu_nice_time,
            &cpu_system_time,
            &cpu_idle_time,
            &cpu_iowait_time,
            &cpu_irq_time,
            &cpu_softirq_time,
            &cpu_steal_time,
            &cpu_guest_time,
            &cpu_guest_nice
            );
        }
        
        cpu_total_time = cpu_user_time + cpu_nice_time + cpu_system_time + cpu_idle_time + cpu_iowait_time + cpu_irq_time + cpu_softirq_time + cpu_steal_time + cpu_guest_time + cpu_guest_nice;

        cpu_info[i].cpu_data.cpu_total_time_period = cpu_info[i].cpu_data.cpu_total_time;
        cpu_info[i].cpu_data.cpu_user_time_period = cpu_info[i].cpu_data.cpu_user_time;
        cpu_info[i].cpu_data.cpu_nice_time_period = cpu_info[i].cpu_data.cpu_nice_time;
        cpu_info[i].cpu_data.cpu_system_time_period = cpu_info[i].cpu_data.cpu_system_time;
        cpu_info[i].cpu_data.cpu_idle_time_period = cpu_info[i].cpu_data.cpu_idle_time;
        cpu_info[i].cpu_data.cpu_iowait_time_period = cpu_info[i].cpu_data.cpu_iowait_time;
        cpu_info[i].cpu_data.cpu_irq_time_period = cpu_info[i].cpu_data.cpu_irq_time;
        cpu_info[i].cpu_data.cpu_softirq_time_period = cpu_info[i].cpu_data.cpu_softirq_time;
        cpu_info[i].cpu_data.cpu_steal_time_period = cpu_info[i].cpu_data.cpu_steal_time;
        cpu_info[i].cpu_data.cpu_guest_time_period = cpu_info[i].cpu_data.cpu_guest_time;
        cpu_info[i].cpu_data.cpu_guest_nice_period = cpu_info[i].cpu_data.cpu_guest_nice;

        cpu_info[i].cpu_data.cpu_total_time = cpu_total_time;
        cpu_info[i].cpu_data.cpu_user_time = cpu_user_time;
        cpu_info[i].cpu_data.cpu_nice_time = cpu_nice_time;
        cpu_info[i].cpu_data.cpu_system_time = cpu_system_time;
        cpu_info[i].cpu_data.cpu_idle_time = cpu_idle_time;
        cpu_info[i].cpu_data.cpu_iowait_time = cpu_iowait_time;
        cpu_info[i].cpu_data.cpu_irq_time = cpu_irq_time;
        cpu_info[i].cpu_data.cpu_softirq_time = cpu_softirq_time;
        cpu_info[i].cpu_data.cpu_steal_time = cpu_steal_time;
        cpu_info[i].cpu_data.cpu_guest_time = cpu_guest_time;
        cpu_info[i].cpu_data.cpu_guest_nice = cpu_guest_nice;

        diff_total_time = cpu_info[i].cpu_data.cpu_total_time - cpu_info[i].cpu_data.cpu_total_time_period;
        diff_idle_time = cpu_info[i].cpu_data.cpu_idle_time - cpu_info[i].cpu_data.cpu_idle_time_period;
        diff_idle_time += cpu_info[i].cpu_data.cpu_iowait_time - cpu_info[i].cpu_data.cpu_iowait_time_period;

        if(diff_total_time != 0)
            cpu_info[i].usage = (double)(diff_total_time - diff_idle_time) / (double)diff_total_time * 100;
        else
            cpu_info[i].usage = 0.0;


    }

    fclose(file_pointer);

    for(int i = 0; i < cpu_count; i++)
    {
        char buffer[BUFFSIZE];
        char address[BUFFSIZE];
        sprintf(address, "%s%d%s",CPU ,i, CPU_CLOCK);
        file_pointer = fopen(address, "r");
        if(file_pointer == NULL)
        {
            printf("Error: CPU file open error\n");
            return NULL;
        }

        fgets(buffer, BUFFSIZE - 1, file_pointer);
        
        sscanf(buffer, "%llu", &(cpu_info[i+1].clock));
        
    }

    fclose(file_pointer);

    file_pointer = fopen(CPU_TEMP, "r");
    
    if(file_pointer == NULL)
    {
        printf("Error: CPU file open error\n");
        return NULL;
    }

    fgets(buffer, BUFFSIZE - 1, file_pointer);
    int therm_temp;
    sscanf(buffer, "%d", &therm_temp);
    cpu_info[0].temp= therm_temp/1000;

    fclose(file_pointer);
    

    return 1;
}

//램 사용량을 읽어오는 함수
int getRamInfo(Hw_info *this)
{
    char buffer[BUFFSIZE];
    FILE *file_pointer;

    file_pointer = fopen(RAM, "r");

    if(file_pointer == NULL)
    {
        printf("Error: RAM file open error\n");
        return 0;
    }

    while(fgets(buffer, BUFFSIZE - 1, file_pointer))
    {
        
        if(strncmp(buffer, "MemTotal:", 8) == 0)
        {
            sscanf(buffer, "%*s %llu", &(this->ram_info.total));
        }
        else if(strncmp(buffer, "MemFree:", 8) == 0)
        {
            sscanf(buffer, "%*s %llu", &(this->ram_info.free));
        }
         else if(strncmp(buffer, "MemAvailable:",8) == 0)
        {
            sscanf(buffer, "%*s %llu", &(this->ram_info.available));
        }
        
    }
    fclose(file_pointer);

    this->ram_info.used = this->ram_info.total - this->ram_info.free;
    this->ram_info.usage = (double)(this->ram_info.used) / (double)this->ram_info.total * 100;

    return 1;
}
//네트워크 사용량을 읽어오는 함수
int getNetworkInfo(Hw_info *this)
{
    char buffer[BUFFSIZE];
    long long unsigned int receive_bytes, transmit_bytes;
    Network_info *network_info = &(this->network_info);
    FILE *file_pointer;

    file_pointer = fopen(NETWORK, "r");

    if(file_pointer == NULL)
    {
        printf("Error: NETWORK file open error\n");
        return 0;
    }

    while(fgets(buffer, BUFFSIZE - 1, file_pointer))
    {
        if(strstr(buffer, "eth0"))
        {
            sscanf(buffer, "%*s %*llu %*llu %*llu %*llu %*llu %*llu %*llu %llu %*llu %*llu %*llu %*llu %*llu %*llu %*llu", 
            &receive_bytes,
            &transmit_bytes);
        }
    }

    fclose(file_pointer);

    network_info->receive_bytes_period = network_info->receive_bytes;
    network_info->transmit_bytes_period = network_info->transmit_bytes;
    network_info->receive_bytes = receive_bytes;
    network_info->transmit_bytes = transmit_bytes;

    network_info->receive_per_second_period = network_info->receive_per_second;
    network_info->transmit_per_second_period = network_info->transmit_per_second;
    network_info->receive_per_second = network_info->receive_bytes - network_info->receive_bytes_period;
    network_info->transmit_per_second = network_info->transmit_bytes - network_info->transmit_bytes_period;

    if (network_info->receive_per_second_period != 0)
    {
        network_info->receive_percent = (double)network_info->receive_per_second / (double)network_info->receive_per_second_period * 100;
    }else
    {
        network_info->receive_percent = 0.0;
    }
    
    if(network_info->transmit_per_second_period != 0)
    {
        network_info->transmit_percent = (double)network_info->transmit_per_second / (double)network_info->transmit_per_second_period * 100;
    }else
    {
        network_info->transmit_percent = 0.0;
    }
    
    
    return 1;
} 