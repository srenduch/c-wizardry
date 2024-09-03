#ifndef __VARIABLE_KEYWORDS_H__
#define __VARIABLE_KEYWORDS_H__

#include "word_def.h"

struct word variable_keywords[] = 
{
    {
        .name = "int",
        .type = VAR_TYPE,
    },
    {
        .name = "char",
        .type = VAR_TYPE
    },
    {
        .name = "bool",
        .type = VAR_TYPE
    },
};

#endif /* __VARIABLE_KEYWORDS_H__ */
