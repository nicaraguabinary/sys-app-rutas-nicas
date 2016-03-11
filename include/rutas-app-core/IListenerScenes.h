//
//  IListenerScenes.h
//  rutas-app-core
//
//  Created by Nicaragua Binary on 11/03/2016.
//  Copyright (c) 2016 NIBSA. All rights reserved.
//

#ifndef IListenerScenes_H
#define IListenerScenes_H

class IListenerScenes {
	public:
		virtual ~IListenerScenes(){
			//
		}
		//
		virtual void loadMap() = 0;
};

#endif
