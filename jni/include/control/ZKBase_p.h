/*
 * ZKBase_p.h
 *
 *  Created on: Jan 8, 2018
 *      Author: guoxs
 */

#ifndef _CONTROL_ZKBASE_PRIVATE_H_
#define _CONTROL_ZKBASE_PRIVATE_H_

#include "ZKBase.h"

inline ZKBaseData::~ZKBaseData() { };

class ZKBasePrivate : public ZKBaseData {
	ZK_DECLARE_PUBLIC(ZKBase)

public:
	virtual ~ZKBasePrivate() { }
};

#endif /* _CONTROL_ZKBASE_PRIVATE_H_ */
