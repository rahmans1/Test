#ifndef __REMOLLGENTEST_HH 
#define __REMOLLGENTEST_HH 
/*!
 * Optical event generator
 *
 * Sakib Rahman
 * January 1, 2019
 *
*/

#include "remollVEventGen.hh"

class remollGenTest : public remollVEventGen {
    public:
	remollGenTest();
	virtual ~remollGenTest();

    private:
	void SamplePhysics(remollVertex *, remollEvent *);

	double fZpos;
};

#endif//__REMOLLGENTEST_HH 
