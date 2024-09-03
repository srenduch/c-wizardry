#ifndef __WORD_DEF_H__
#define __WORD_DEF_H__

enum word_type
{
	MACRO_KEYWORD,
	MACRO_KEY,
	MACRO_VALUE,
	VAR_TYPE,
	VAR_NAME,
    OPERATOR_LOGICAL,
    OPERATOR_COMPARE,
    OPERATOR_ASSIGNMENT,
    OPEARTOR_ARITHMETIC,
	WORD_TYPE_MAX,
	WORD_TYPE_INVALID = WORD_TYPE_MAX
};

struct word
{
	enum word_type type;
	char* name;
};


#endif /* __WORDS_H__ */
