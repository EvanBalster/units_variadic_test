#pragma once

namespace v_dimensions
{
	// SI units
	struct length      { static const int DIM_ID = 100; static const char *name() {return "Length";} };
	struct mass        { static const int DIM_ID = 200; static const char *name() {return "Mass";} };
	struct time        { static const int DIM_ID = 300; static const char *name() {return "Time";} };
	struct charge      { static const int DIM_ID = 400; static const char *name() {return "charge";} };
	struct temperature { static const int DIM_ID = 500; static const char *name() {return "Temperature";} };
	struct substance   { static const int DIM_ID = 600; static const char *name() {return "Substance";} };
	struct luminance   { static const int DIM_ID = 700; static const char *name() {return "Luminance";} };

	// Useful nonstandard units
	struct angle       { static const int DIM_ID = 10100; static const char *name() {return "Angle";} };
	struct data_length { static const int DIM_ID = 10200; static const char *name() {return "Data Length";} };

	// Easy to create user-defined types as well.
}
