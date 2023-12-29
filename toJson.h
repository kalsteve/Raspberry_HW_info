#ifndef _TOJSON_H_
#define _TOJSON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <json-c/json.h>
#include "hwmoniter.h"

json_object* hwInfoToJson(Hw_info *);
#endif