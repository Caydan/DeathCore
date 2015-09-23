/*
 * Copyright (C) 2013-2015 DeathCore <http://www.noffearrdeathproject.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _PERFORMANCE_LOG_H_
#define _PERFORMANCE_LOG_H_

#include "Common.h"
#include <ace/Singleton.h>

// Comment this to disable logging of everything but world::update
//#define PERFORMANCELOG_ENABLED

#ifdef PERFORMANCELOG_ENABLED
#   define PERFLOG_CREATE(name, lengthID, variable) PerformanceEntry variable(name, lengthID)
#   define PERFLOG_TIME(variable, text) variable.LogTime(text)
#   define PERFLOG_FINISH(variable) sPerfLog->Add(&variable)
#   define PERFLOG_INFO(variable, text, ...) variable.AddInfo(text, __VA_ARGS__)
#   define PERFLOG_INFO_CONST(variable, text) variable.AddInfo(text)
#else
#   define PERFLOG_CREATE(name, lengthID, variable)
#   define PERFLOG_TIME(variable, text)
#   define PERFLOG_FINISH(variable)
#   define PERFLOG_INFO(variable, text, ...)
#   define PERFLOG_INFO_CONST(variable, text)
#endif

#define MAX_PERFORMANCELOG_TYPEID 0xFF

// Template class (Don't use directly)
class PerformanceEntry
{
friend class PerformanceLog;
public:
    PerformanceEntry(const char *name, uint8 lengthID);
    void LogTime(const char* name);
    void AddInfo(const char *format, ...);
    void Finish();

    const char* GetType() { return m_name; }
    const uint8 GetTypeId() { return m_id; }

protected:
    char m_name[255];
    uint8 m_id;

    uint32 m_totalTime;
    uint32 m_lastTime;
    
    std::stringstream m_times;
    std::stringstream m_info;
};

class PerformanceLog
{
public:
    // General
    void Initialize();
    void LoadConfig();
    void Update(uint32 diff);
    void Add(PerformanceEntry *pe);

private:
    // World update diff
    uint32 m_worldCount;
    uint32 m_worldSum;
    uint32 m_worldMax;

    // Config
    uint32 m_config[MAX_PERFORMANCELOG_TYPEID];
};

#define sPerfLog ACE_Singleton<PerformanceLog, ACE_Null_Mutex>::instance()

#endif