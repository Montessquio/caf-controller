#pragma once

#define delay(_CYCLES) for (volatile int d = 0; d < _CYCLES; d++);
