//
//  AppDelegate.h
//  app-refraneronica-osxapp
//
//  Created by Marcos Ortega on 06/06/14.
//  Copyright (c) 2014 NIBSA. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "AUGLView.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (assign) IBOutlet NSWindow *window;
@property (assign) IBOutlet AUGLView *vistaGL;

@end
