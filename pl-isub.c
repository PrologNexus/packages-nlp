/*  Part of SWI-Prolog

    Author:        Jan Wielemaker
    E-mail:        J.Wielemaker@vu.nl
    WWW:           http://www.swi-prolog.org
    Copyright (c)  2011-2015, VU University Amsterdam
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in
       the documentation and/or other materials provided with the
       distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
    ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/

#include <config.h>
#include <SWI-Prolog.h>
#include "isub.h"
#include "wcsdup.ic"

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
isub_score_inplace() modifies the contents.  That is why we use
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#define FAST_SIZE 64

static int
get_chars(term_t t, wchar_t **sb, wchar_t *buf)
{ wchar_t *s;
  size_t len;

  if ( PL_get_wchars(t, &len, &s, CVT_ATOMIC|CVT_LIST|CVT_EXCEPTION) )
  { if ( len+1 < FAST_SIZE )
    { *sb = wcscpy(buf, s);
    } else
    { if ( !(*sb = wcsdup(s)) )
	return FALSE;
    }

    return TRUE;
  }

  return FALSE;
}


static foreign_t
pl_isub_(term_t t1, term_t t2, term_t normalize, term_t sim, int allowShort)
{ wchar_t buf1[FAST_SIZE];
  wchar_t buf2[FAST_SIZE];
  wchar_t *s1=NULL, *s2=NULL;
  int rc;
  int normaliseStrings;

  if ( !get_chars(t1, &s1, buf1) ||
       !get_chars(t2, &s2, buf2) ||
       !PL_get_bool_ex(normalize, &normaliseStrings) )
  { rc = FALSE;
    goto out;
  }

  rc = PL_unify_float(sim, isub_score_inplace(s1, s2, normaliseStrings, allowShort));

out:
  if ( s1 && s1 != buf1 ) PL_free(s1);
  if ( s2 && s2 != buf2 ) PL_free(s2);

  return rc;
}

static foreign_t
pl_isub(term_t t1, term_t t2, term_t normalize, term_t sim)
{  int allowShort = 0;
   return pl_isub_(t1, t2, normalize, sim, allowShort);
}

static foreign_t
pl_isub_short(term_t t1, term_t t2, term_t normalize, term_t sim)
{  int allowShort = 1;
   return pl_isub_(t1,  t2, normalize, sim, allowShort);
}


install_t
install_isub()
{ PL_register_foreign("isub", 4, pl_isub, 0);
  PL_register_foreign("isub_short", 4, pl_isub_short, 0);
}
