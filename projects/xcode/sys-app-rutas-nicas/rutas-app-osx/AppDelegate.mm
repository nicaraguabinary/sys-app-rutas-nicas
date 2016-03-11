//
//  AppDelegate.m
//  app-refraneronica-osxapp
//
//  Created by Marcos Ortega on 06/06/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//

#import "AppDelegate.h"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	// Insert code here to initialize your application
	/*NSRect r;
	r.size.width = 1920; r.size.height = 1080 + 24; //+24 de la barra
	r.origin.x = -r.size.width; r.origin.y = 0;
	[[self window] setFrame:r display:YES];*/
}

/*- (BOOL)application:(NSApplication *)theApplication openFile:(NSString *)filename {
	AUGLView* vistaGL = self.vistaGL;
	if(vistaGL != nil){
		return [vistaGL abrirArchivo: filename];
	}
	return NO;
}*/

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)theApplication {
    return YES;
}

@end
