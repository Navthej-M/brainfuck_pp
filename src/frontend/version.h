#ifndef _VERSION_H
#define _VERSION_H

#define _stringfy_helper(x) #x
#define _stringfy(x) _stringfy_helper(x)

#define bfppc_major 1
#define bfppc_minor 0
#define bfppc_patch 0

#define bfppc_version_string                                                   \
    _stringfy(bfppc_major) "." _stringfy(bfppc_minor) "." _stringfy(bfppc_patch)

#endif
