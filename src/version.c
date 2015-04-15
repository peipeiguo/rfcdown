#include "version.h"

void
rfcdown_version(int *major, int *minor, int *revision)
{
	*major = RFCDOWN_VERSION_MAJOR;
	*minor = RFCDOWN_VERSION_MINOR;
	*revision = RFCDOWN_VERSION_REVISION;
}
