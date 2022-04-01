// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

#include "targetver.h"

#include <assert.h>
#include <stdio.h>
#include <tchar.h>
#include <vector>
#include <iostream>
#include <algorithm> 
#include <assert.h>
#include <numeric>
#include <iomanip>  

typedef  std::vector<float> vecFloat;
typedef  std::vector<int> vecInt;


//#define SELF_ADAPTIVE_PRICING  // this is the flag for system vs self // if commented out, then SYSTEM

using namespace std;

#define PRINT_INTERVAL 50 // days before it prints to screen - doesn't affect simulation, just less screen printing which makes it a bit quicker to run

