/**
 * Author: paladin_t, hellotony521@gmail.com
 * Created: Oct. 14, 2011
 * Last edited: Feb. 28, 2012
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://sam.zoy.org/wtfpl/COPYING for more details.
 */

#include "xpl.h"

static int _xpl_is_rsolidus(unsigned char _c) {
  return _c == '\\';
}

static int _xpl_parse_escape(char** _d, const char** _s) {
  int ret = 0;
  if(*++*_s) {
    switch(*(*_s)++) {
      case '"':
        *(*_d)++ = '"'; ret++;
        break;
      case '\\':
        *(*_d)++ = '\\'; ret++;
        break;
      case '/':
        *(*_d)++ = '/'; ret++;
        break;
      case 'b':
        *(*_d)++ = '\b'; ret++;
        break;
      case 'f':
        *(*_d)++ = '\f'; ret++;
        break;
      case 'n':
        *(*_d)++ = '\n'; ret++;
        break;
      case 'r':
        *(*_d)++ = '\r'; ret++;
        break;
      case 't':
        *(*_d)++ = '\t'; ret++;
        break;
      default:
        break;
    }
  }
  
  return ret;
}

static xpl_status_t test1(xpl_context_t* _s) {
  double f = 0.0;
  printf("test1\n");
  if(xpl_has_param(_s) == XS_OK) {
    xpl_pop_double(_s, &f);
    printf("has_param %f\n", f);
  }

  return XS_OK;
}

static xpl_status_t test2(xpl_context_t* _s) {
  char buf[64] = { '\0' };
  printf("test2\n");
  if(xpl_has_param(_s) == XS_OK) {
    xpl_pop_string(_s, buf, 64);
    printf("has_param %s\n", buf);
  }

  return XS_OK;
}

static xpl_status_t test3(xpl_context_t* _s) {
  printf("test3\n");

  return XS_OK;
}

static xpl_status_t cond1(xpl_context_t* _s) {
  printf("cond1\n");
  xpl_push_bool(_s, 0);

  return XS_OK;
}

static xpl_status_t cond2(xpl_context_t* _s) {
  printf("cond2\n");
  xpl_push_bool(_s, 1);

  return XS_OK;
}

static xpl_context_t xpl;

int main() {
  XPL_FUNC_BEGIN(funcs)
    XPL_FUNC_ADD("test3", test3)
    XPL_FUNC_ADD("test2", test2)
    XPL_FUNC_ADD("test1", test1)
    XPL_FUNC_ADD("cond2", cond2)
    XPL_FUNC_ADD("cond1", cond1)
  XPL_FUNC_END

  xpl_open(&xpl, funcs, NULL);
    xpl.escape_detect = _xpl_is_rsolidus;
    xpl.escape_parse = _xpl_parse_escape;
    xpl_load(&xpl, "if cond1 then test1 3.14 elseif cond2 then test2 \"hello world\" else test3 endif");
    xpl_run(&xpl);
    xpl_load(&xpl, "if cond1 then if cond2 3 then test3 elseif cond2 then test3 endif test3 endif test2 \"hello world\"");
    xpl_run(&xpl);
    xpl_unload(&xpl);
  xpl_close(&xpl);

  return 0;
}
