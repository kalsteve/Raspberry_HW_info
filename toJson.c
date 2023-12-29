#include "toJson.h"

json_object* hwInfoToJson(Hw_info *this)
{
    Cpu_info *cpu_info = this->cpu_info;
    Disk_info *disk_info = this->disk_info;
    Ram_info *ram_info = &this->ram_info;
    Network_info *network_info = &this->network_info;
    board_info *board_info = &this->board_info;
    int cpu_count = this->cpu_count;
    int disk_count = this->disk_count;
    json_object 
    *hw_info_object, 
    *cpu_info_object,
    *cpu_data_object,
    *disk_info_object,
    *disk_data_object,
    *board_info_object,
    *ram_info_object,
    *network_info_object;

    char buffer[512];
    memset(buffer, 0, sizeof(buffer));


    if(this == NULL)
    {
        return NULL;
    }

    hw_info_object = json_object_new_object();

    //cpu_count를 json으로 변환
    //memset(buffer, 0, sizeof(buffer));
    //sprintf(buffer, "%d", cpu_count);
    //json_object_object_add(hw_info_object, "cpu_count", json_object_new_string(buffer));

    //disk_count를 json으로 변환
    //memset(buffer, 0, sizeof(buffer));
    //sprintf(buffer, "%d", disk_count);
    //json_object_object_add(hw_info_object, "disk_count", json_object_new_string(buffer));

    //board 정보를 json으로 변환
    board_info_object = json_object_new_object();
    json_object_object_add(board_info_object, "model", json_object_new_string(board_info->model));
    json_object_object_add(board_info_object, "serial_number", json_object_new_string(board_info->serial_number));
    json_object_object_add(board_info_object, "hardware", json_object_new_string(board_info->hardware));
    json_object_object_add(board_info_object, "version", json_object_new_string(board_info->version));
    json_object_object_add(board_info_object, "linux_version", json_object_new_string(board_info->linux_version));
    
    json_object_object_add(hw_info_object, "BOARD_INFO", board_info_object);

    

    //cpu 데이터를 json으로 변환
    cpu_info_object = json_object_new_array();
    
    
    for(int i=0; i<=cpu_count; i++)
    {
        cpu_data_object = json_object_new_object();
        memset(buffer, 0, sizeof(buffer));
        sprintf(buffer, "%d", i);
        json_object_object_add(cpu_data_object,"CPU_ID", json_object_new_string(buffer));
        sprintf(buffer, "%.2f", cpu_info[i].usage);
        json_object_object_add(cpu_data_object, "usage", json_object_new_string(buffer));
        sprintf(buffer, "%llu", cpu_info[i].clock);
        json_object_object_add(cpu_data_object, "clock", json_object_new_string(buffer));
        sprintf(buffer, "%.2f", cpu_info[0].temp);
        json_object_object_add(cpu_data_object, "temp", json_object_new_string(buffer));
        
        json_object_array_add(cpu_info_object, cpu_data_object);
    }

    json_object_object_add(hw_info_object, "CPU_INFO", cpu_info_object);

    //disk 데이터를 json으로 변환
    disk_info_object = json_object_new_array();

    for(int i=0; i<disk_count; i++)
    {
        disk_data_object = json_object_new_object();
        json_object_object_add(disk_data_object,"DISK_ID", json_object_new_string(disk_info[i].name));
        sprintf(buffer, "%.2f", disk_info[i].usage);
        json_object_object_add(disk_data_object, "usage", json_object_new_string(buffer));
        sprintf(buffer, "%llu", disk_info[i].total_size);
        json_object_object_add(disk_data_object, "total_size", json_object_new_string(buffer));
        sprintf(buffer, "%llu", disk_info[i].used_size);
        json_object_object_add(disk_data_object, "used_size", json_object_new_string(buffer));
        sprintf(buffer, "%llu", disk_info[i].free_size);
        json_object_object_add(disk_data_object, "free_size", json_object_new_string(buffer));
    
        
        json_object_array_add(disk_info_object, disk_data_object);
    }

    json_object_object_add(hw_info_object, "DISK_INFO", disk_info_object);

    //ram 데이터를 json으로 변환
    ram_info_object = json_object_new_object();
    sprintf(buffer, "%.2f", ram_info->usage);
    json_object_object_add(ram_info_object, "usage", json_object_new_string(buffer));
    sprintf(buffer, "%llu", ram_info->total);
    json_object_object_add(ram_info_object, "total", json_object_new_string(buffer));
    sprintf(buffer, "%llu", ram_info->used);
    json_object_object_add(ram_info_object, "used", json_object_new_string(buffer));
    sprintf(buffer, "%llu", ram_info->free);
    json_object_object_add(ram_info_object, "free", json_object_new_string(buffer));

    json_object_object_add(hw_info_object, "RAM_INFO", ram_info_object);

    //network 데이터를 json으로 변환
    network_info_object = json_object_new_object();
    sprintf(buffer, "%llu", network_info->receive_per_second);
    json_object_object_add(network_info_object, "receive_per_second", json_object_new_string(buffer));
    sprintf(buffer, "%llu", network_info->transmit_per_second);
    json_object_object_add(network_info_object, "transmit_per_second", json_object_new_string(buffer));
    sprintf(buffer, "%.f", network_info->receive_percent);
    json_object_object_add(network_info_object, "receive_percent", json_object_new_string(buffer));
    sprintf(buffer, "%.f", network_info->transmit_percent);
    json_object_object_add(network_info_object, "transmit_percent", json_object_new_string(buffer));

    json_object_object_add(hw_info_object, "NETWORK_INFO", network_info_object);

    return hw_info_object;


}