/**
 * Author: paladin_t, hellotony521@gmail.com
 * Created: Oct. 14, 2011
 * Last edited: May. 29, 2013
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#ifndef __XPL_H__
#define __XPL_H__

#ifdef _MSC_VER
#  pragma warning(disable : 4100)
#  pragma warning(disable : 4127)
#  pragma warning(disable : 4201)
#  pragma warning(disable : 4706)
#endif /* _MSC_VER */

#ifdef __APPLE__
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunused-function"
#endif /* __APPLE__ */

#include <memory.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
** {========================================================
** Macros and typedefines
*/

#ifndef XPLVER
#  define XPLVER_MAJOR  1
#  define XPLVER_MINOR  0
#  define XPLVER_PATCH  17
#  define XPLVER ((XPLVER_MAJOR << 24) | (XPLVER_MINOR << 16) | (XPLVER_PATCH))
#endif /* !XPLVER */

#ifndef XPLINTERNAL
#  define XPLINTERNAL static
#endif /* !XPLINTERNAL */
#ifndef XPLAPI
#  define XPLAPI static
#endif /* !XPLAPI */

#ifndef NULL
#  define NULL 0
#endif /* !NULL */

#ifndef _countof
#  define _countof(a) (sizeof(a) / sizeof(*(a)))
#endif /* !_countof */

#ifndef xpl_assert
#  define xpl_assert(e) assert(e)
#endif /* !xpl_assert */

/**
 * @brief XPL scripting programming interface registering macros
 * @note The interfaces are storaged in a common array, you could put these
 *  macros at global or local scopes to make your customized interfaces.
 */
#ifndef XPL_FUNC_REGISTER
#  define XPL_FUNC_REGISTER
/**< Begins an interface declaration without buildin interfaces. */
#  define XPL_FUNC_BEGIN_EMPTY(a) \
    static xpl_func_info_t a[] = {
/**< Begins an interface declaration with buildin interfaces. */
#  define XPL_FUNC_BEGIN(a) \
    static xpl_func_info_t a[] = { \
      { "if", _xpl_core_if }, \
      { "then", _xpl_core_then }, \
      { "elseif", _xpl_core_elseif }, \
      { "else", _xpl_core_else }, \
      { "endif", _xpl_core_endif }, \
      { "or", _xpl_core_or }, \
      { "and", _xpl_core_and }, \
      { "yield", _xpl_core_yield },
/**< Declares an interface. */
#  define XPL_FUNC_ADD(n, f) \
      { n, f },
/**< Ends an interface declaration. */
#  define XPL_FUNC_END \
      { NULL, NULL }, \
    };
#endif /* !XPL_FUNC_REGISTER */

/**
 * @brief Skips meaningless parts, such as comment and blank.
 */
#ifndef XPL_SKIP_MEANINGLESS
#  define XPL_SKIP_MEANINGLESS(s) \
    do { \
      while(_xpl_is_squote(*(unsigned char*)(s)->cursor) || _xpl_is_blank(*(unsigned char*)(s)->cursor)) { \
        _xpl_trim(&(s)->cursor); if(xpl_skip_comment(s) == XS_OK) _xpl_trim(&(s)->cursor); } \
    } while(0)
#endif /* !XPL_SKIP_MEANINGLESS */

/**
 * @brief Avoids function folding optimization during compiling time.
 */
#ifndef XPL_DO_NOTHING
#  define XPL_DO_NOTHING(s) do { (s)->pfunc_hack += __LINE__; } while(0)
#endif /* !XPL_DO_NOTHING */

/**
 * @brief XPL function execution status.
 */
typedef enum xpl_status_t {
  XS_OK,                    /**< Totally OK. */
  XS_SUSPENT,               /**< Suspent. */
  XS_ERR,                   /**< Some error occured. */
  XS_NO_ENOUGH_BUFFER_SIZE, /**< String buffer too small. */
  XS_NO_COMMENT,            /**< No comment found. */
  XS_NO_PARAM,              /**< No param found. */
  XS_PARAM_TYPE_ERROR,      /**< Parameter convertion failed. */
  XS_BAD_ESCAPE_FORMAT,     /**< Bad escape format. */
  XS_COUNT
} xpl_status_t;

/**
 * @brief Boolean value composing type.
 */
typedef enum xpl_bool_composing_t {
  XBC_NIL, /**< Assigns with a new value directly. */
  XBC_OR,  /**< Composes and assigns old value OR a new value. */
  XBC_AND  /**< Composes and assigns old value AND a new value. */
} xpl_bool_composing_t;

struct xpl_context_t;

/**
 * @brief XPL scripting programming interface signature.
 *
 * @param[in] _s - XPL context.
 */
typedef xpl_status_t (* xpl_func_t)(struct xpl_context_t* _s);

/**
 * @brief XPL scripting programming interface information.
 */
typedef struct xpl_func_info_t {
  const char* name; /**< Interface name. */
  xpl_func_t func;  /**< Pointer to interface function. */
} xpl_func_info_t;

/**
 * @brief Separator determination functor.
 *
 * @param[in] _c - Charactor to be determinated.
 * @return - Returns non-zero if separator determinated.
 */
typedef int (* xpl_is_separator_func)(unsigned char _c);
/**
 * @brief Escape determination functor.
 *
 * @param[in] _c - Charactor to be determinated.
 * @return - Returns non-zero if escape determinated.
 */
typedef int (* xpl_is_escape_func)(unsigned char _c);

/**
 * @brief Escape parser.
 *
 * @param[in] _d - Pointer to destination buffer.
 * @param[in] _s - Pointer to source buffer.
 * @return - Returns parsed escape charactor count.
 */
typedef int (* xpl_parse_escape_func)(char** _d, const char** _s);

/**
 * @brief XPL context structure.
 */
typedef struct xpl_context_t {
  /**
   * @brief Registered interfaces.
   */
  /* {===== */
    xpl_func_info_t* funcs; /**< Pointer to array of registered interfaces. */
    int funcs_count;        /**< Count of registered interfaces. */
  /* =====} */
  /**
   * @brief Script source code indicator.
   */
  /* {===== */
    const char* text;   /**< Script source text. */
    const char* cursor; /**< Script execution cursor. */
  /* =====} */
  /**
   * @brief Boolean value.
   */
  /* {===== */
    xpl_bool_composing_t bool_composing; /**< Boolean value composing type. */
    int bool_value;                      /**< Current boolean value. */
  /* =====} */
  /**
   * @brief Nest logic helper.
   */
  /* {===== */
    int if_statement_depth; /**< 'if' statement depth. */
  /* =====} */
  /**
   * @brief Separator determination functor.
   */
  xpl_is_separator_func separator_detect;
  /**
   * @brief Escape determination functor.
   */
  xpl_is_escape_func escape_detect;
  /**
   * @brief Escape parser.
   */
  xpl_parse_escape_func escape_parse;
  /**
   * @brief Pointer to user defined data.
   */
  void* userdata;
  /**
   * @brief Used to avoids function folding optimization during compiling time.
   */
  /* {===== */
    int use_hack_pfunc; /**< Use pfunc hacking if non-zero. */
    int pfunc_hack;     /**< Dummy function hack. */
  /* =====} */
} xpl_context_t;

/* ========================================================} */

/*
** {========================================================
** Function declarations
*/

/**
 * @brief Opens an XPL context.
 *
 * @param[in] _s  - XPL context.
 * @param[in] _f  - Pointer to XPL scripting interface array.
 * @param[in] _is - Separator determination functor.
 * @return - Returns execution status.
 */
XPLAPI xpl_status_t xpl_open(xpl_context_t* _s, xpl_func_info_t* _f, xpl_is_separator_func _is);
/**
 * @brief Closes an XPL context.
 *
 * @param[in] _s - XPL context.
 * @return - Returns execution status.
 */
XPLAPI xpl_status_t xpl_close(xpl_context_t* _s);

/**
 * @brief Loads a script.
 *
 * @param[in] _s - XPL context.
 * @return - Returns execution status.
 */
XPLAPI xpl_status_t xpl_load(xpl_context_t* _s, const char* _t);
/**
 * @brief Reloads current script, set execution cursor to the beginning.
 *
 * @param[in] _s - XPL context.
 * @return - Returns execution status.
 */
XPLAPI xpl_status_t xpl_reload(xpl_context_t* _s);
/**
 * @brief Unloads a script.
 *
 * @param[in] _s - XPL context.
 * @return - Returns execution status.
 */
XPLAPI xpl_status_t xpl_unload(xpl_context_t* _s);

/**
 * @brief Runs a script.
 *
 * @param[in] _s - XPL context.
 * @return - Returns execution status.
 */
XPLAPI xpl_status_t xpl_run(xpl_context_t* _s);
/**
 * @brief Tries to peek one function.
 *
 * @param[in] _s - XPL context.
 * @return - Returns execution status.
 */
XPLAPI xpl_status_t xpl_peek_func(xpl_context_t* _s, xpl_func_info_t** _f);
/**
 * @brief Runs a single step.
 *
 * @param[in] _s - XPL context.
 * @return - Returns execution status.
 */
XPLAPI xpl_status_t xpl_step(xpl_context_t* _s);

/**
 * @brief Skips a piece of comment.
 *
 * @param[in] _s - XPL context.
 * @return - Returns execution status.
 */
XPLAPI xpl_status_t xpl_skip_comment(xpl_context_t* _s);
/**
 * @brief Determines whether there's a parameter in current XPL context at
 *  cursor point.
 *
 * @param[in] _s - XPL context.
 * @return - Returns execution status, XS_OK if there's a parameter, or
 *  XS_NO_PARAM if not.
 */
XPLAPI xpl_status_t xpl_has_param(xpl_context_t* _s);
/**
 * @brief Skips a string parameter from XPL context.
 *
 * @param[in] _s - XPL context.
 * @return - Returns execution status, XS_OK if succeed.
 */
XPLAPI xpl_status_t xpl_skip_string(xpl_context_t* _s);
/**
 * @brief Pops a long integer parameter from XPL context.
 *
 * @param[in] _s  - XPL context.
 * @param[out] _o - Destination buffer.
 * @return - Returns execution status.
 */
XPLAPI xpl_status_t xpl_pop_long(xpl_context_t* _s, long* _o);
/**
 * @brief Pops a double float parameter from XPL context.
 *
 * @param[in] _s  - XPL context.
 * @param[out] _o - Destination buffer.
 * @return - Returns execution status.
 */
XPLAPI xpl_status_t xpl_pop_double(xpl_context_t* _s, double* _o);
/**
 * @brief Pops a string parameter from XPL context.
 *
 * @param[in] _s  - XPL context.
 * @param[out] _o - Destination buffer.
 * @param[in] _l  - Destination buffer size.
 * @return - Returns execution status.
 */
XPLAPI xpl_status_t xpl_pop_string(xpl_context_t* _s, char* _o, int _l);
/**
 * @brief Pushes a boolean value to XPL context.
 *
 * @param[in] _s - XPL context.
 * @param[in] _b - Boolean value.
 * @return - Returns execution status.
 */
XPLAPI xpl_status_t xpl_push_bool(xpl_context_t* _s, int _b);

/**
 * @brief Scripting programming interface:
 *   'if' statement, dummy function.
 *
 * @param[in] _s - XPL context.
 * @return - Returns execution status.
 */
XPLINTERNAL xpl_status_t _xpl_core_if(xpl_context_t* _s);
/**
 * @brief Scripting programming interface:
 *   'then' statement, main logic about 'if-then-elseif-else-endif'.
 *
 * @param[in] _s - XPL context.
 * @return - Returns execution status.
 */
XPLINTERNAL xpl_status_t _xpl_core_then(xpl_context_t* _s);
/**
 * @brief Scripting programming interface:
 *   'elseif' statement, dummy function.
 *
 * @param[in] _s - XPL context.
 * @return - Returns execution status.
 */
XPLINTERNAL xpl_status_t _xpl_core_elseif(xpl_context_t* _s);
/**
 * @brief Scripting programming interface:
 *   'else' statement, dummy function.
 *
 * @param[in] _s - XPL context.
 * @return - Returns execution status.
 */
XPLINTERNAL xpl_status_t _xpl_core_else(xpl_context_t* _s);
/**
 * @brief Scripting programming interface:
 *   'endif' statement, dummy function.
 *
 * @param[in] _s - XPL context.
 * @return - Returns execution status.
 */
XPLINTERNAL xpl_status_t _xpl_core_endif(xpl_context_t* _s);
/**
 * @brief Scripting programming interface:
 *   'or' statement, set current boolean composing type as OR.
 *
 * @param[in] _s - XPL context.
 * @return - Returns execution status.
 */
XPLINTERNAL xpl_status_t _xpl_core_or(xpl_context_t* _s);
/**
 * @brief Scripting programming interface:
 *   'and' statement, set current boolean composing type as AND.
 *
 * @param[in] _s - XPL context.
 * @return - Returns execution status.
 */
XPLINTERNAL xpl_status_t _xpl_core_and(xpl_context_t* _s);
/**
 * @brief Scripting programming interface:
 *   'yield' statement, suspend current execution.
 *
 * @param[in] _s - XPL context.
 * @return - Returns execution status.
 */
XPLINTERNAL xpl_status_t _xpl_core_yield(xpl_context_t* _s);

/**
 * @brief Skips execution body of an 'if' statement.
 *
 * @param[in] _s - XPL context.
 */
XPLINTERNAL void _xpl_skip_ifcond_body(xpl_context_t* _s);

/**
 * @brief Determines whether a char is a single quote.
 *
 * @param[in] _c - Char to be determined.
 * @return - Returns non-zero if matching.
 */
XPLINTERNAL int _xpl_is_squote(unsigned char _c);
/**
 * @brief Determines whether a char is a double quote.
 *
 * @param[in] _c - Char to be determined.
 * @return - Returns non-zero if matching.
 */
XPLINTERNAL int _xpl_is_dquote(unsigned char _c);
/**
 * @brief Determines whether a char is a comma.
 *
 * @param[in] _c - Char to be determined.
 * @return - Returns non-zero if matching.
 */
XPLINTERNAL int _xpl_is_comma(unsigned char _c);
/**
 * @brief Determines whether a char is an exclamation.
 *
 * @param[in] _c - Char to be determined.
 * @return - Returns non-zero if matching.
 */
XPLINTERNAL int _xpl_is_exclamation(unsigned char _c);
/**
 * @brief Determines whether a char is a colon.
 *
 * @param[in] _c - Char to be determined.
 * @return - Returns non-zero if matching.
 */
XPLINTERNAL int _xpl_is_colon(unsigned char _c);
/**
 * @brief Determines whether a char is a blank.
 *
 * @param[in] _c - Char to be determined.
 * @return - Returns non-zero if matching.
 */
XPLINTERNAL int _xpl_is_blank(unsigned char _c);
/**
 * @brief Determines whether a char is a separator.
 *
 * @param[in] _c  - Char to be determined.
 * @param[in] _is - Separator determination functor.
 * @return - Returns non-zero if matching.
 */
XPLINTERNAL int _xpl_is_separator(unsigned char _c, xpl_is_separator_func _is);
/**
 * @brief Trims extra chars.
 *
 * @param[in][out] - String to be trimmed.
 * @return - Returns count of trimmed chars.
 */
XPLINTERNAL int _xpl_trim(const char** _c);
/**
 * @brief Compires two strings.
 *
 * @param[in] _s - Source string.
 * @param[in] _d - Destination buffer.
 * @return - Returns 1 if _s > _d, -1 if _s < _d, 0 if _s = _d.
 */
XPLINTERNAL int _xpl_strcmp(const char* _s, const char* _d);
/**
 * @brief Compires scripting programming interface information in quick
 *  sorting.
 *
 * @param[in] _l - First interface information.
 * @param[in] _r - Second interface information.
 * @return - Returns 1 if _l > _r, -1 if _l < _r, 0 if _l = _r.
 */
XPLINTERNAL int _xpl_func_info_srt_cmp(const void* _l, const void* _r);
/**
 * @brief Compires scripting programming interface information in binary
 *  searching.
 *
 * @param[in] _k - Searching key.
 * @param[in] _i - Interface information.
 * @return - Returns 1 if _k > _i, -1 if _k < _i, 0 if _k = _i.
 */
XPLINTERNAL int _xpl_func_info_sch_cmp(const void* _k, const void* _i);

/* ========================================================} */

/*
** {========================================================
** Function definitions
*/

XPLAPI xpl_status_t xpl_open(xpl_context_t* _s, xpl_func_info_t* _f, xpl_is_separator_func _is) {
  xpl_assert(_s && _f);
  memset(_s, 0, sizeof(xpl_context_t));
  _s->funcs = _f;
  while(_f[_s->funcs_count].name && _f[_s->funcs_count].func)
    _s->funcs_count++;
  qsort(_f, _s->funcs_count, sizeof(xpl_func_info_t), _xpl_func_info_srt_cmp);
  _s->separator_detect = _is;
  _s->use_hack_pfunc = 1;

  return XS_OK;
}

XPLAPI xpl_status_t xpl_close(xpl_context_t* _s) {
  xpl_assert(_s);
  if(_s->use_hack_pfunc)
    printf("XPL closed, pfunc_hack code: %d\n", _s->pfunc_hack);
  memset(_s, 0, sizeof(xpl_context_t));

  return XS_OK;
}

XPLAPI xpl_status_t xpl_load(xpl_context_t* _s, const char* _t) {
  xpl_assert(_s && _t);
  if(_s->text) xpl_unload(_s);
  _s->cursor = _s->text = _t;

  return XS_OK;
}

XPLAPI xpl_status_t xpl_reload(xpl_context_t* _s) {
  xpl_assert(_s && _s->text);
  _s->cursor = _s->text;

  return XS_OK;
}

XPLAPI xpl_status_t xpl_unload(xpl_context_t* _s) {
  xpl_assert(_s);
  _s->cursor = _s->text = NULL;

  return XS_OK;
}

XPLAPI xpl_status_t xpl_run(xpl_context_t* _s) {
  xpl_status_t ret = XS_OK;
  xpl_assert(_s && _s->text && "Empty program");
  while(*_s->cursor && ret == XS_OK)
    ret = xpl_step(_s);

  return ret;
}

XPLAPI xpl_status_t xpl_peek_func(xpl_context_t* _s, xpl_func_info_t** _f) {
  xpl_status_t ret = XS_OK;
  xpl_func_info_t* func = NULL;
  xpl_assert(_s && _s->text);
  XPL_SKIP_MEANINGLESS(_s);
  if(_f) *_f = NULL;
  if(_xpl_is_comma(*(unsigned char*)_s->cursor)) {
    _s->cursor++;
  } else {
    func = (xpl_func_info_t*)bsearch(_s->cursor, _s->funcs, _s->funcs_count, sizeof(xpl_func_info_t), _xpl_func_info_sch_cmp);
    if(!func) return XS_ERR;
    if(_f) *_f = func;
  }

  return ret;
}

XPLAPI xpl_status_t xpl_step(xpl_context_t* _s) {
  xpl_status_t ret = XS_OK;
  xpl_func_info_t* func = NULL;
  xpl_assert(_s && _s->text);
  if((ret = xpl_peek_func(_s, &func)) != XS_OK) return ret;
  if(!func) return ret;
  _s->cursor += strlen(func->name);
  XPL_SKIP_MEANINGLESS(_s);
  if((ret = func->func(_s)) != XS_OK) return ret;

  return ret;
}

XPLAPI xpl_status_t xpl_skip_comment(xpl_context_t* _s) {
  xpl_assert(_s && _s->text);
  if(_xpl_is_squote(*(unsigned char*)_s->cursor)) {
    do {
      _s->cursor++;
    } while(*(unsigned char*)_s->cursor != '\0' && !_xpl_is_squote(*(unsigned char*)_s->cursor));
    _s->cursor++;

    return XS_OK;
  }

  return XS_NO_COMMENT;
}

XPLAPI xpl_status_t xpl_has_param(xpl_context_t* _s) {
  xpl_func_info_t* func = NULL;
  xpl_assert(_s && _s->text);
  XPL_SKIP_MEANINGLESS(_s);
  if(_s->cursor[0] == '\0') return XS_NO_PARAM;
  xpl_peek_func(_s, &func);

  return ((_xpl_is_comma(*(unsigned char*)_s->cursor) || func) ? XS_NO_PARAM : XS_OK);
}

XPLAPI xpl_status_t xpl_skip_string(xpl_context_t* _s) {
  const char* src = NULL;
  xpl_assert(_s && _s->text);
  src = _s->cursor;
  if(_xpl_is_dquote(*(unsigned char*)src)) {
    src++;
    while(!_xpl_is_dquote(*(unsigned char*)src))
      src++;
    src++;
  } else {
    while(!_xpl_is_separator(*(unsigned char*)src, _s->separator_detect) && *src != '\0')
      src++;
  }
  _s->cursor = src;

  return XS_OK;
}

XPLAPI xpl_status_t xpl_pop_long(xpl_context_t* _s, long* _o) {
  xpl_status_t ret = XS_OK;
  char* conv_suc = NULL;
  char buf[32] = { '\0' };
  xpl_assert(_s && _s->text && _o);
  if((ret = xpl_pop_string(_s, buf, sizeof(buf))) != XS_OK) return ret;
  *_o = strtol(buf, &conv_suc, 0);
  if(*conv_suc != '\0') ret = XS_PARAM_TYPE_ERROR;

  return ret;
}

XPLAPI xpl_status_t xpl_pop_double(xpl_context_t* _s, double* _o) {
  xpl_status_t ret = XS_OK;
  char* conv_suc = NULL;
  char buf[32] = { '\0' };
  xpl_assert(_s && _s->text && _o);
  if((ret = xpl_pop_string(_s, buf, sizeof(buf))) != XS_OK) return ret;
  *_o = strtod(buf, &conv_suc);
  if(*conv_suc != '\0') ret = XS_PARAM_TYPE_ERROR;

  return ret;
}

XPLAPI xpl_status_t xpl_pop_string(xpl_context_t* _s, char* _o, int _l) {
  const char* src = NULL;
  char* dst = NULL;
  xpl_assert(_s && _s->text && _o);
  src = _s->cursor;
  dst = _o;
  if(_xpl_is_dquote(*(unsigned char*)src)) {
    src++;
    while(!_xpl_is_dquote(*(unsigned char*)src)) {
      if(_s->escape_detect && (*_s->escape_detect)(*(unsigned char*)src)) {
        xpl_assert(_s->escape_parse);
        if(!(*_s->escape_parse)(&dst, &src))
          return XS_BAD_ESCAPE_FORMAT;
      } else {
        *dst++ = *src++;
      }
      if(dst + 1 - _o > _l) return XS_NO_ENOUGH_BUFFER_SIZE;
    }
    src++;
  } else {
    while(!_xpl_is_separator(*(unsigned char*)src, _s->separator_detect) && *src != '\0') {
      *dst++ = *src++;
      if(dst + 1 - _o > _l) return XS_NO_ENOUGH_BUFFER_SIZE;
    }
  }
  _s->cursor = src;
  if(dst + 1 - _o > _l) return XS_NO_ENOUGH_BUFFER_SIZE;
  *dst++ = '\0';

  return XS_OK;
}

XPLAPI xpl_status_t xpl_push_bool(xpl_context_t* _s, int _b) {
  xpl_assert(_s && _s->text);
  switch(_s->bool_composing) {
    case XBC_NIL: _s->bool_value = !!_b; break;
    case XBC_OR: _s->bool_value |= !!_b; break;
    case XBC_AND: _s->bool_value &= !!_b; break;
    default: xpl_assert(!"Unknow boolean composing type"); break;
  }

  return XS_OK;
}

XPLINTERNAL xpl_status_t _xpl_core_if(xpl_context_t* _s) {
  xpl_assert(_s && _s->text);
  _s->if_statement_depth++;

  return XS_OK;
}

XPLINTERNAL xpl_status_t _xpl_core_then(xpl_context_t* _s) {
  xpl_status_t ret = XS_OK;
  xpl_func_info_t* func = NULL;
  xpl_assert(_s && _s->text);
  if(_s->bool_value) {
    _s->bool_value = 0;
    _s->bool_composing = XBC_NIL;
    do {
      if((ret = xpl_peek_func(_s, &func)) != XS_OK) return ret;
      if(!func) continue;
      if(func->func == _xpl_core_elseif || func->func == _xpl_core_else) break;
      else if(func->func == _xpl_core_endif) { _s->if_statement_depth--; break; }
      _s->cursor += strlen(func->name);
      XPL_SKIP_MEANINGLESS(_s);
      if((ret = func->func(_s)) != XS_OK) return ret;
    } while(*_s->cursor);
    do {
      if((ret = xpl_peek_func(_s, &func)) != XS_OK) return ret;
      if(!func) continue;
      _s->cursor += strlen(func->name);
      if(func->func == _xpl_core_endif) { _s->if_statement_depth--; break; }
    } while(*_s->cursor);
  } else {
    _s->bool_value = 0;
    _s->bool_composing = XBC_NIL;
    do {
      _xpl_skip_ifcond_body(_s);
      xpl_peek_func(_s, &func);
      if(!func) continue;
      if(func->func == _xpl_core_elseif || func->func == _xpl_core_else) break;
      else if(func->func == _xpl_core_endif) { _s->if_statement_depth--; break; }
      _s->cursor += strlen(func->name);
    } while(*_s->cursor);
  }

  return ret;
}

XPLINTERNAL xpl_status_t _xpl_core_elseif(xpl_context_t* _s) {
  xpl_assert(_s && _s->text);
  XPL_DO_NOTHING(_s);

  return XS_OK;
}

XPLINTERNAL xpl_status_t _xpl_core_else(xpl_context_t* _s) {
  xpl_assert(_s && _s->text);
  XPL_DO_NOTHING(_s);

  return XS_OK;
}

XPLINTERNAL xpl_status_t _xpl_core_endif(xpl_context_t* _s) {
  xpl_assert(_s && _s->text);
  XPL_DO_NOTHING(_s);

  return XS_OK;
}

XPLINTERNAL xpl_status_t _xpl_core_or(xpl_context_t* _s) {
  xpl_assert(_s && _s->text);
  _s->bool_composing = XBC_OR;

  return XS_OK;
}

XPLINTERNAL xpl_status_t _xpl_core_and(xpl_context_t* _s) {
  xpl_assert(_s && _s->text);
  _s->bool_composing = XBC_AND;

  return XS_OK;
}

XPLINTERNAL xpl_status_t _xpl_core_yield(xpl_context_t* _s) {
  xpl_assert(_s && _s->text);

  return XS_SUSPENT;
}

XPLINTERNAL void _xpl_skip_ifcond_body(xpl_context_t* _s) {
  xpl_func_info_t* func = NULL;
  int lv = _s->if_statement_depth;
  xpl_assert(_s && _s->text);
  do {
    xpl_peek_func(_s, &func);
    if(!func) {
      XPL_SKIP_MEANINGLESS(_s);
      _s->cursor++;
      while(!_xpl_is_separator(*(unsigned char*)(_s->cursor), _s->separator_detect))
        _s->cursor++;
      continue;
    } else if(func->func == _xpl_core_if) {
      _s->if_statement_depth++;
    } else if(func->func == _xpl_core_elseif || func->func == _xpl_core_else || func->func == _xpl_core_endif) {
      if(_s->if_statement_depth == lv) break;
      else if(func->func == _xpl_core_endif) _s->if_statement_depth--;
    }
    _s->cursor += strlen(func->name);
  } while(*_s->cursor);
}

XPLINTERNAL int _xpl_is_squote(unsigned char _c) {
  return _c == '\'';
}

XPLINTERNAL int _xpl_is_dquote(unsigned char _c) {
  return _c == '"';
}

XPLINTERNAL int _xpl_is_comma(unsigned char _c) {
  return _c == ',';
}

XPLINTERNAL int _xpl_is_exclamation(unsigned char _c) {
  return _c == '!';
}

XPLINTERNAL int _xpl_is_colon(unsigned char _c) {
  return _c == ':';
}

XPLINTERNAL int _xpl_is_blank(unsigned char _c) {
  return _c == ' ' || _c == '\t' || _c == '\r' || _c == '\n';
}

XPLINTERNAL int _xpl_is_separator(unsigned char _c, xpl_is_separator_func _is) {
  return _xpl_is_blank(_c) || _xpl_is_comma(_c) ||
    _xpl_is_colon(_c) ||
    _xpl_is_squote(_c) || _xpl_is_dquote(_c) ||
    (_is ? _is(_c) : 0);
}

XPLINTERNAL int _xpl_trim(const char** _c) {
  int ret = 0;
  xpl_assert(_c && *_c);
  while(_xpl_is_blank(**_c)) {
    ret++;
    (*_c)++;
  }

  return ret;
}

XPLINTERNAL int _xpl_strcmp(const char* _s, const char* _d) {
  int ret = 0;
  while(!(ret = (_xpl_is_separator(*(unsigned char*)_s, NULL) ? '\0' : *(unsigned char*)_s) - *(unsigned char*)_d) && *_d) {
    _s++; _d++;
  }
  if(ret < 0) ret = -1;
  else if(ret > 0) ret = 1;

  return ret;
}

XPLINTERNAL int _xpl_func_info_srt_cmp(const void* _l, const void* _r) {
  xpl_func_info_t* l = (xpl_func_info_t*)_l;
  xpl_func_info_t* r = (xpl_func_info_t*)_r;
  xpl_assert(l && r);

  return _xpl_strcmp(l->name, r->name);
}

XPLINTERNAL int _xpl_func_info_sch_cmp(const void* _k, const void* _i) {
  const char* k = (const char*)_k;
  xpl_func_info_t* i = (xpl_func_info_t*)_i;
  xpl_assert(k && i);

  return _xpl_strcmp(k, i->name);
}

/* ========================================================} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#ifdef __APPLE__
#   pragma clang diagnostic pop
#endif /* __APPLE__ */

#endif /* !__XPL_H__ */
