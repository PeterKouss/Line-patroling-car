#ifndef __IIRFILTER_H__
#define __IIRFILTER_H__

#define IIR_ODER (4)

const float IIR_B[IIR_ODER] = {
	0.001735764203807, 
	0.005207292611421, 
	0.005207292611421,
	0.001735764203807
};

const float IIR_A[IIR_ODER] = {
	1,   
	-2.479878730149,    
	2.086485800156,  
	-0.5927209563764
};

#endif
