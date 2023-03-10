#ifndef __VOOTRM_H
# define __VOOTRM_H

# ifdef __VOO_HAS_ANSI_TERMINAL
#  define VOO_CSI                "\033["
#  define VOO_TERM_RESET         VOO_CSI "0m"
#  define VOO_TERM_BOLD          VOO_CSI "1m"
#  define VOO_TERM_ITALIC        VOO_CSI "3m"
#  define VOO_TERM_UNDERLINE     VOO_CSI "4m"
#  define VOO_TERM_FCOL_BLACK    VOO_CSI "30m"
#  define VOO_TERM_FCOL_RED      VOO_CSI "31m"
#  define VOO_TERM_FCOL_GREEN    VOO_CSI "32m"
#  define VOO_TERM_FCOL_YELLOW   VOO_CSI "33m"
#  define VOO_TERM_FCOL_BLUE     VOO_CSI "34m"
#  define VOO_TERM_FCOL_MAGENTA  VOO_CSI "35m"
#  define VOO_TERM_FCOL_CYAN     VOO_CSI "36m"
#  define VOO_TERM_FCOL_WHITE    VOO_CSI "37m"
#  define VOO_TERM_FCOL_DEF      VOO_CSI "39m"
#  define VOO_TERM_BCOL_BLACK    VOO_CSI "40m"
#  define VOO_TERM_BCOL_RED      VOO_CSI "41m"
#  define VOO_TERM_BCOL_GREEN    VOO_CSI "42m"
#  define VOO_TERM_BCOL_YELLOW   VOO_CSI "43m"
#  define VOO_TERM_BCOL_BLUE     VOO_CSI "44m"
#  define VOO_TERM_BCOL_MAGENTA  VOO_CSI "45m"
#  define VOO_TERM_BCOL_CYAN     VOO_CSI "46m"
#  define VOO_TERM_BCOL_WHITE    VOO_CSI "47m"
#  define VOO_TERM_BCOL_DEF      VOO_CSI "49m"
#  define VOO_TERM_FCOL_LBLACK   VOO_CSI "90m"
#  define VOO_TERM_FCOL_LRED     VOO_CSI "91m"
#  define VOO_TERM_FCOL_LGREEN   VOO_CSI "92m"
#  define VOO_TERM_FCOL_LYELLOW  VOO_CSI "93m"
#  define VOO_TERM_FCOL_LBLUE    VOO_CSI "94m"
#  define VOO_TERM_FCOL_LMAGENTA VOO_CSI "95m"
#  define VOO_TERM_FCOL_LCYAN    VOO_CSI "96m"
#  define VOO_TERM_FCOL_LWHITE   VOO_CSI "97m"
#  define VOO_TERM_BCOL_LBLACK   VOO_CSI "100m"
#  define VOO_TERM_BCOL_LRED     VOO_CSI "101m"
#  define VOO_TERM_BCOL_LGREEN   VOO_CSI "102m"
#  define VOO_TERM_BCOL_LYELLOW  VOO_CSI "103m"
#  define VOO_TERM_BCOL_LBLUE    VOO_CSI "104m"
#  define VOO_TERM_BCOL_LMAGENTA VOO_CSI "105m"
#  define VOO_TERM_BCOL_LCYAN    VOO_CSI "106m"
#  define VOO_TERM_BCOL_LWHITE   VOO_CSI "107m"
# else
#  define VOO_CSI
#  define VOO_TERM_RESET
#  define VOO_TERM_BOLD
#  define VOO_TERM_ITALIC
#  define VOO_TERM_UNDERLINE
#  define VOO_TERM_FCOL_BLACK
#  define VOO_TERM_FCOL_RED
#  define VOO_TERM_FCOL_GREEN
#  define VOO_TERM_FCOL_YELLOW
#  define VOO_TERM_FCOL_BLUE
#  define VOO_TERM_FCOL_MAGENTA
#  define VOO_TERM_FCOL_CYAN
#  define VOO_TERM_FCOL_WHITE
#  define VOO_TERM_FCOL_DEF
#  define VOO_TERM_BCOL_BLACK
#  define VOO_TERM_BCOL_RED
#  define VOO_TERM_BCOL_GREEN
#  define VOO_TERM_BCOL_YELLOW
#  define VOO_TERM_BCOL_BLUE
#  define VOO_TERM_BCOL_MAGENTA 
#  define VOO_TERM_BCOL_CYAN
#  define VOO_TERM_BCOL_WHITE
#  define VOO_TERM_BCOL_DEF
#  define VOO_TERM_FCOL_LBLACK
#  define VOO_TERM_FCOL_LRED
#  define VOO_TERM_FCOL_LGREEN
#  define VOO_TERM_FCOL_LYELLOW
#  define VOO_TERM_FCOL_LBLUE
#  define VOO_TERM_FCOL_LMAGENTA
#  define VOO_TERM_FCOL_LCYAN
#  define VOO_TERM_FCOL_LWHITE
#  define VOO_TERM_BCOL_LBLACK
#  define VOO_TERM_BCOL_LRED
#  define VOO_TERM_BCOL_LGREEN
#  define VOO_TERM_BCOL_LYELLOW
#  define VOO_TERM_BCOL_LBLUE
#  define VOO_TERM_BCOL_LMAGENTA
#  define VOO_TERM_BCOL_LCYAN 
#  define VOO_TERM_BCOL_LWHITE
# endif

#endif