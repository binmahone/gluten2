/* *
*  Nothing, just make folly compile
*  Fix:
*    folly/io/async/AsyncSocket.cpp:47:10: fatal error: 'linux/sockios.h' file not found
*/

#pragma once

#define SIOCINQ		FIONREAD
#define SIOCOUTQ	TIOCOUTQ
