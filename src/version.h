/* version.h - holds Hoedown's version */

#ifndef RFCDOWN_VERSION_H
#define RFCDOWN_VERSION_H

#ifdef __cplusplus
extern "C" {
#endif


/*************
 * CONSTANTS *
 *************/

#define RFCDOWN_VERSION "1.0.0"
#define RFCDOWN_VERSION_MAJOR 1
#define RFCDOWN_VERSION_MINOR 0
#define RFCDOWN_VERSION_REVISION 0


/*************
 * FUNCTIONS *
 *************/

/* rfcdown_version: retrieve Hoedown's version numbers */
void rfcdown_version(int *major, int *minor, int *revision);


#ifdef __cplusplus
}
#endif

#endif /** RFCDOWN_VERSION_H **/
