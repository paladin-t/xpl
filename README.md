# Introduction
XPL is an easy to embed and extend scripting programming language. It's implemented in a single C header file within only several hundreds lines of code; and runs almost as fast as `strlen()`. It contains only a few high frequently used features like: `if-then-elseif-else-endif`, `yield`, scripting interface invoking etc. Registering the scripting interface is as easy as writing a common array. The design principle of XPL is doing 80% of work with 20% of core code, doing left work with few extended scripting interface. It's aimed to be a thin and light weight scripting solution.

# Syntax Tutorials
~~~~~~~~~~basic
'
  This is a short tutorial on XPL syntax shows you some basic usage.
  You could make your own dialects by extending it.
  After all, this is a comment between single quotes.
'
if you_like_xpl then
  try_use_xpl,
  leave_a_feedback "Your suggestions, my pleasure"
elseif you_do_not_like_xpl and you_have_some_time then
  try_read_xpl,
  leave_a_advice "Your suggestions, my pleasure"
else
  leave_a_idea "Anytime, my pleasure"
endif
~~~~~~~~~~

# License
XPL is distributed under [DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE](http://sam.zoy.org/wtfpl/COPYING).
