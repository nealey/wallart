#include <Timezone.h>

TimeChangeRule usEDT = {"EDT", Second, Sun, Mar, 2, -240};
TimeChangeRule usEST = {"EST", First, Sun, Nov, 2, -300};
Timezone TZ_US_Eastern(usEDT, usEST);

TimeChangeRule usCDT = {"CDT", Second, Sun, Mar, 2, -300};
TimeChangeRule usCST = {"CST", First, Sun, Nov, 2, -360};
Timezone TZ_US_Central(usCDT, usCST);

TimeChangeRule usMDT = {"MDT", Second, Sun, Mar, 2, -360};
TimeChangeRule usMST = {"MST", First, Sun, Nov, 2, -420};
Timezone TZ_US_Mountain(usMDT, usMST);

TimeChangeRule usPDT = {"EDT", Second, Sun, Mar, 2, -420};
TimeChangeRule usPST = {"EST", First, Sun, Nov, 2, -480};
Timezone TZ_US_Pacific(usPDT, usPST);
