#ifndef __MACRO_KEYWORDS_H__
#define __MACRO_KEYWORDS_H__

#include "word_def.h"

struct word macro_keywords[] =
{
    {
        .type = MACRO_KEYWORD,
        .name = "#ifndef"
    },
    {
        .type = MACRO_KEYWORD,
        .name = "#define"
    },
    {
        .type = MACRO_KEYWORD,
        .name = "#endif"
    },
    {
        .type = MACRO_KEYWORD,
        .name = "#elif"
    },
    {
        .type = MACRO_KEYWORD,
        .name = "#else"
    },
};

#endif /* __MACRO_KEYWORDS_H__ */
