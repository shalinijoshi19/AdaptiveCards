//
//  ACRTextView
//  ACRTextView.h
//
//  Copyright © 2018 Microsoft. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "ACRIBaseInputHandler.h"
#import "ACOBaseCardElement.h"

@interface ACRTextView:UITextView<ACRIBaseInputHandler, UITextViewDelegate>
@property NSString* id;
@property NSString* placeholderText;
@property bool isRequired;
@property NSUInteger maxLength;
@property IBInspectable UIColor* borderColor;

- (instancetype)initWithFrame:(CGRect)frame element:(ACOBaseCardElement *)element;
- (void)configWithSharedModel:(ACOBaseCardElement *)element;

@end
