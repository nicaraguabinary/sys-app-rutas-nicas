//
//  AUGLView.h
//  app-refranero
//
//  Created by Marcos Ortega on 10/05/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//

#import <Cocoa/Cocoa.h>

// for display link
#import <QuartzCore/QuartzCore.h>

@interface AUGLView : NSOpenGLView {
	@private
	CVDisplayLinkRef		displayLink;
	//double				deltaTime;
	//double				outputTime;
	//
	AUApp*					_app;
	AUScenesAdmin*			_scenes;
	//SI32					_indiceEscenaRender;
	bool					_mousePresionado;
	NSMutableString*		_rutaArchivoCargar;
	NBHILO_CLASE			_hiloPorSegundo;
};

@property (assign) IBOutlet NSWindow *ventana;

- (BOOL) abrirArchivo:(NSString*)rutaArchivo;

void *funcHiloTickUnSegundo(void* datos);

@end

