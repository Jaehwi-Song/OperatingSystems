#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char* argv[])
{
 int gpid = getgpid();
 printf(1, "My gpid is %d\n", gpid);
 exit();
}
