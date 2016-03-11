//
//  AUGLView.m
//  app-refranero
//
//  Created by Marcos Ortega on 10/05/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//

#import "AUGLView.h"

@interface AUGLView (InternalMethods)

- (void)initcializarGL:(NSRect)frame;
- (CVReturn)getFrameForTime:(const CVTimeStamp *)outputTime;
- (void)drawFrame;

@end

@implementation AUGLView

static CVReturn MyDisplayLinkCallback(CVDisplayLinkRef displayLink, const CVTimeStamp *now, const CVTimeStamp *outputTime, CVOptionFlags flagsIn, CVOptionFlags *flagsOut, void *displayLinkContext){
	// go back to Obj-C for easy access to instance variables
	CVReturn result = [(AUGLView*)displayLinkContext getFrameForTime:outputTime];
	return result;
}

- (void)awakeFromNib {
	_rutaArchivoCargar = [[NSMutableString alloc] init];
	//
	STAppCallbacks appCallbacks;			AUApp::inicializarCallbacks(&appCallbacks);
	_app		= new AUApp(&appCallbacks, "RutasNicas");
	if(!AUApp::inicializarGraficos(LEER_PRECACHE, LEER_CACHE, ESCRIBIR_CACHE, 60)){
		PRINTF_ERROR("No se pudo inicializar el motor grafico\n"); NBASSERT(false);
	} else {
		/*{
			NSArray* pantallas  = [NSScreen screens];
			NSUInteger i; PRINTF_INFO("PANTALLAS conteo(%d).\n", (SI32)[pantallas count]);
			for(i = 0; i < [pantallas count]; i++){
				NSScreen* pantalla = (NSScreen*)[pantallas objectAtIndex:i];
				NSDictionary* descripcion = [pantalla deviceDescription];
				const NSSize ppp = [[descripcion objectForKey:NSDeviceResolution] sizeValue];
				const NSSize tamano = [[descripcion objectForKey:NSDeviceSize] sizeValue];
				PRINTF_INFO("Pantalla #%d dpi(%f, %f) size(%f, %f).\n", (SI32)(i + 1), ppp.width, ppp.height, tamano.width, tamano.height);
			}
		}*/
		//
		NSDictionary* deviceDescription = [[self window] deviceDescription];
		NSSize resolution = [[deviceDescription objectForKey:NSDeviceResolution] sizeValue]; PRINTF_INFO("Resolucion de plantalla dpi(%f, %f).\n", resolution.width, resolution.height);
		NSSize tamano = [[deviceDescription objectForKey:NSDeviceSize] sizeValue]; PRINTF_INFO("Tamano de plantalla dpi(%f, %f).\n", tamano.width, tamano.height);
		if(resolution.width < 160.0f) resolution.width = 160.0f;
		if(resolution.height < 160.0f) resolution.height = 160.0f;
		//Referencia: Monitor Benq G2220HDA a (1920 x 1080)pix es (102.5 x 102.5)dpi
		//
		NSSize    viewBounds	= [self bounds].size;
		if(!_app->inicializarVentana(viewBounds.width, viewBounds.height, resolution.width, resolution.height, ENGestorEscenaDestinoGl_Heredado)){
			PRINTF_ERROR("No se pudo inicializar la ventana.\n");
		} else {
			NBASSERT(_app->indiceEscenaRender() >= 0)
			const char* paquetes[] = {
				"paqFuentes.otf"
				, "paqPNGx4.png"
				, "paqPNGx2.png"
				, "paqPNGx1.png"
				, "paqAnimaciones.xml"
				, "paqSonidos.wav"
				, "paqMusicaOgg.wav"
			};
			const SI32 iPrimerPaqueteCargar = 0;
			const SI32 conteoPaquetesCargar = 7;
			//
			_scenes = new AUScenesAdmin(_app->indiceEscenaRender(), PAQUETES_RUTA_BASE, &paquetes[iPrimerPaqueteCargar], conteoPaquetesCargar);
			if(!_app->inicializarJuego(_scenes)){
				PRINTF_ERROR("Couldnt init app.\n");
			} else {
				PRINTF_INFO("App inited!\n");
				{
					//
					//Generar el cache de fuentes
					//
					/*
					 -------------------------
					 PPP Device
					 -------------------------
					 120	Base, Android-low y iPad1-2
					 160	iPhone y android-medium
					 200	---
					 240	Android-high y iPad3-4
					 280	---
					 320	Iphone-retina, iPadMini2-3 y Android-extra-high
					 360	---
					 400	iPhone 6Plus
					 440	--
					 480	Android-extra-extra-high
					 520	---
					 560	---
					 600	---
					 640	Android-extra-extra-extra-high
					 */
				}
				//Hilo tickUnSegundo
				int rTickUnSeg = pthread_create(&_hiloPorSegundo, NULL, funcHiloTickUnSegundo, &_app);
				if(rTickUnSeg!=0){
					PRINTF_ERROR("#%d en pthread_create, no se pudo crear el HILO funcHiloTickUnSegundo.\n", rTickUnSeg);
					NBASSERT(false) //no se pudo crear el hilo de animacion
				} else {
					PRINTF_INFO("HILO funcHiloTickUnSegundo creado.\n");
				}
			}
		}
	}
	//
	// activate the display link
	CVDisplayLinkStart(displayLink);
}

- (id)initWithCoder:(NSCoder*)aDecoder {
	self = [super initWithCoder:aDecoder];
	if (self){
		[self initcializarGL: [self frame]];
	}
	return self;
}

- (id)initWithFrame:(NSRect)frame {
	[self initcializarGL: frame];
	return self;
}

- (void)initcializarGL:(NSRect)frame {
	// context setup
	NSOpenGLPixelFormatAttribute attribs[] = {
		NSOpenGLPFAWindow,
		NSOpenGLPFAColorSize, 32,
		NSOpenGLPFAAccelerated,
		NSOpenGLPFADoubleBuffer,
		NSOpenGLPFASingleRenderer,
		0 };
	NSOpenGLPixelFormat* windowedPixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attribs];
	if (windowedPixelFormat == nil) {
		NSLog(@"Unable to create windowed pixel format.");
		exit(0);
	}
	self = [super initWithFrame:frame pixelFormat:windowedPixelFormat];
	if (self == nil) {
		NSLog(@"Unable to create a windowed OpenGL context.");
		exit(0);
	}
	[windowedPixelFormat release];
	//
	// set synch to VBL to eliminate tearing
	GLint    vblSynch = 1; [[self openGLContext] setValues:&vblSynch forParameter:NSOpenGLCPSwapInterval];
	// set up the display link
	CVDisplayLinkCreateWithActiveCGDisplays(&displayLink);
	CVDisplayLinkSetOutputCallback(displayLink, MyDisplayLinkCallback, self);
	CGLContextObj cglContext = (CGLContextObj)[[self openGLContext] CGLContextObj];
	CGLPixelFormatObj cglPixelFormat = (CGLPixelFormatObj)[[self pixelFormat] CGLPixelFormatObj];
	CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(displayLink, cglContext, cglPixelFormat);
}

- (BOOL) abrirArchivo:(NSString*)rutaArchivo {
	PRINTF_INFO("application:openURL: '%s'.\n", [rutaArchivo UTF8String]);
	[_rutaArchivoCargar setString:rutaArchivo];
	return YES;
}

- (CVReturn)getFrameForTime:(const CVTimeStamp *)outputTime {
	// deltaTime is unused in this bare bones demo, but here's how to calculate it using display link info
	//deltaTime = 1.0 / (outputTime->rateScalar * (double)outputTime->videoTimeScale / (double)outputTime->videoRefreshPeriod);
	[self drawFrame];
	return kCVReturnSuccess;
}

- (void)reshape {
	NSSize    viewBounds = [self bounds].size;
	NSOpenGLContext    *currentContext = [self openGLContext];
	if([currentContext view] != nil){
		[currentContext makeCurrentContext];
		// remember to lock the context before we touch it since display link is threaded
		CGLLockContext((CGLContextObj)[currentContext CGLContextObj]);
		// let the context know we've changed size
		[[self openGLContext] update];
		//
		if(_app != NULL) _app->notificarRedimensionPantalla(viewBounds.width, viewBounds.height);
		CGLUnlockContext((CGLContextObj)[currentContext CGLContextObj]);
	}
}

- (void)drawRect:(NSRect)rect {
	[self drawFrame];
}

- (void)drawFrame {
	NSOpenGLContext* currentContext = [self openGLContext];
	NSView* view = [currentContext view];
	if(view != nil){
		[currentContext makeCurrentContext];
		// must lock GL context because display link is threaded
		CGLLockContext((CGLContextObj)[currentContext CGLContextObj]);
		if(_app != NULL){
			//Bug en OSX (depende del SDK y el OS):
			//A veces el frameBuffer es undefined al arancar el App o al cambiar a pantalla completa.
			//Para evitar que el primer glClear produzca error, se debe verificar el estado del frameBuffer heredado (fbId=0)
			NBGestorGL::bindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
			const GLenum estadoFBO = NBGestorGL::checkFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
			if(estadoFBO == GL_FRAMEBUFFER_COMPLETE_EXT){
				_app->tickJuego(ENAUAppTickTipo_SincPantalla, false);
			}
		}
		[currentContext flushBuffer];
		CGLUnlockContext((CGLContextObj)[currentContext CGLContextObj]);
	}
}

- (void)dealloc {
	CVDisplayLinkRelease(displayLink);
	if(_app!=NULL){
		AUApp::finalizarGraficos();
		delete _app;
		_app = NULL;
	}
	[_rutaArchivoCargar release];
	[super dealloc];
}

// ------------------------------
// -- Manejo de eventos de teclado
// ------------------------------

- (void)keyDown:(NSEvent *)theEvent {
    [self interpretKeyEvents:[NSArray arrayWithObject:theEvent]];
}

- (void)insertNewline:(id)sender {
	NBGestorTeclas::teclaIntroducirTexto("\n");
}

- (void)insertTab:(id)sender {
	NBGestorTeclas::teclaIntroducirTexto("\t");
}

- (void)insertText:(id)aString {
	NBGestorTeclas::teclaIntroducirTexto([aString UTF8String]);
}

- (void)deleteBackward:(id)sender {
	NBGestorTeclas::teclaBackspace();
}

// ------------------------------
// -- Manejo de eventos de mouse
// ------------------------------
- (BOOL)acceptsFirstResponder {
    return YES;
}


- (void)mouseDown:(NSEvent *)theEvent {
	const NSSize viewBounds	= [self bounds].size;
    const NSPoint curPoint	= [self convertPoint:[theEvent locationInWindow] fromView:nil];
	const float yPosInv		= (viewBounds.height - curPoint.y);
	//PRINTF_INFO("mouseDown x(%f) y(%f) - alto(%f) yInv(%f).\n", (float)curPoint.x, (float)curPoint.y, (float)viewBounds.height, (float)yPosInv);
	if(_app != NULL){
		if(_mousePresionado) _app->touchFinalizado(1, curPoint.x, yPosInv, false);
		_app->touchIniciado(1, curPoint.x, yPosInv);
		_mousePresionado = true;
	}
}

- (void)mouseUp:(NSEvent *)theEvent {
	const NSSize viewBounds	= [self bounds].size;
    const NSPoint curPoint	= [self convertPoint:[theEvent locationInWindow] fromView:nil];
	const float yPosInv		= (viewBounds.height - curPoint.y);
	//PRINTF_INFO("mouseUp x(%f) y(%f) - alto(%f) yInv(%f).\n", (float)curPoint.x, (float)curPoint.y, (float)viewBounds.height, (float)yPosInv);
	if(_app != NULL){
		if(_mousePresionado) _app->touchFinalizado(1, curPoint.x, yPosInv, false);
		_mousePresionado = false;
	}
}

- (void)mouseDragged:(NSEvent *)theEvent {
	const NSSize viewBounds	= [self bounds].size;
    const NSPoint curPoint	= [self convertPoint:[theEvent locationInWindow] fromView:nil];
	const float yPosInv		= (viewBounds.height - curPoint.y);
	//PRINTF_INFO("mouseUp x(%f) y(%f) - alto(%f) yInv(%f).\n", (float)curPoint.x, (float)curPoint.y, (float)viewBounds.height, (float)yPosInv);
	if(_app != NULL){
		if(_mousePresionado) _app->touchMovido(1, curPoint.x, yPosInv);
	}
}

@end

void *funcHiloTickUnSegundo(void* datos){
	AUApp* app = *((AUApp**)datos);
	while(true){
		NBHILO_SLEEP_MS(1000);
		if(app != NULL) app->tickUnSegundo();
	}
	return 0;
}

