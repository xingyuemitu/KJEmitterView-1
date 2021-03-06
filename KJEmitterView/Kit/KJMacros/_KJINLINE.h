//
//  _KJINLINE.h
//  KJEmitterView
//
//  Created by 杨科军 on 2019/10/10.
//  Copyright © 2019 杨科军. All rights reserved.
//

#ifndef _KJINLINE_h
#define _KJINLINE_h

#ifdef __OBJC__
#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>
#import <objc/message.h>

/// 这里只适合放简单的函数
NS_ASSUME_NONNULL_BEGIN

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

/// 字符串转换为非空
NS_INLINE NSString * kString(NSString *string){
    return (string ?: @"");
}
/// 字符串是否为空
NS_INLINE bool kEmptyIsString(NSString *string){
    return ([string isKindOfClass:[NSNull class]] || string == nil || ([string length] < 1 ? true:false));
}
/// 数组是否为空
NS_INLINE bool kEmptyIsArray(NSArray *array){
    return (array == nil || [array isKindOfClass:[NSNull class]] || array.count == 0);
}
/// 字典是否为空
NS_INLINE bool kEmptyIsDictionary(NSDictionary *dict){
    return (dict == nil || [dict isKindOfClass:[NSNull class]] || dict.allKeys == 0);
}
/// 对象是否为空
NS_INLINE bool kEmptyIsObject(NSObject *_object){
    return (_object == nil || [_object isKindOfClass:[NSNull class]] || ([_object respondsToSelector:@selector(length)] && [(NSData*)_object length] == 0) || ([_object respondsToSelector:@selector(count)] && [(NSArray*)_object count] == 0));
}
/// 随机颜色
NS_INLINE UIColor * kRandomColor(void){
    return [UIColor colorWithRed:((float)arc4random_uniform(256)/255.0) green:((float)arc4random_uniform(256)/255.0) blue:((float)arc4random_uniform(256)/255.0) alpha:1.0];
}
/// 交换方法的实现
NS_INLINE void kMethod_swizzling(Class clazz, SEL originalSelector, SEL swizzledSelector) {
    Method originalMethod = class_getInstanceMethod(clazz, originalSelector);
    Method swizzledMethod = class_getInstanceMethod(clazz, swizzledSelector);
    if (class_addMethod(clazz, originalSelector, method_getImplementation(swizzledMethod), method_getTypeEncoding(swizzledMethod))) {
        class_replaceMethod(clazz, swizzledSelector, method_getImplementation(originalMethod), method_getTypeEncoding(originalMethod));
    }else {
        method_exchangeImplementations(originalMethod, swizzledMethod);
    }
}

#pragma mark -------------- UI处理 -------------
/// 自定提醒窗口
NS_INLINE UIAlertView * kAlertView(NSString *title, NSString *message, id delegate, NSString *cancelTitle, NSString *otherTitle){
    __block UIAlertView *alerView;
    dispatch_async(dispatch_get_main_queue(), ^{
        alerView = [[UIAlertView alloc] initWithTitle:title message:message delegate:delegate cancelButtonTitle:cancelTitle otherButtonTitles:otherTitle, nil];
        [alerView show];
    });
    return alerView;
}
/// 自定提醒窗口，自动消失
NS_INLINE void kAlertViewAutoDismiss(NSString *message, CGFloat delay){
    dispatch_async(dispatch_get_main_queue(), ^{
        UIAlertView *alerView = [[UIAlertView alloc] initWithTitle:@"提示" message:message delegate:nil cancelButtonTitle:nil otherButtonTitles:nil, nil];
        [alerView show];
        [alerView performSelector:@selector(dismissWithClickedButtonIndex:animated:) withObject:@[@0,@1] afterDelay:delay];
    });
}
/// 系统加载动效
NS_INLINE void kNetworkActivityIndicatorVisible(BOOL visible) {
    [UIApplication sharedApplication].networkActivityIndicatorVisible = visible;
}
/// 通过xib名称加载cell
NS_INLINE id kLoadNibWithName(NSString *name, id owner){
   return [[NSBundle mainBundle] loadNibNamed:name owner:owner options:nil].firstObject;
}
/// 加载xib
NS_INLINE id kLoadNib(NSString *nibName){
    return [UINib nibWithNibName:nibName bundle:[NSBundle mainBundle]];
}
/// 校正ScrollView在iOS11上的偏移问题
NS_INLINE void kAdjustsScrollViewInsetNever(UIViewController *viewController, __kindof UIScrollView *tableView) {
#if __IPHONE_11_0
    if (@available(iOS 11.0, *)) {
        tableView.contentInsetAdjustmentBehavior = UIScrollViewContentInsetAdjustmentNever;
    }else {
        viewController.automaticallyAdjustsScrollViewInsets = false;
    }
#else
    viewController.automaticallyAdjustsScrollViewInsets = false;
#endif
}
/// 根据当前view 找所在tableview 里的 indexpath
NS_INLINE NSIndexPath * kIndexpathSubviewTableview(UIView *subview, UITableView *tableview){
    CGRect subviewFrame = [subview convertRect:subview.bounds toView:tableview];
    return [tableview indexPathForRowAtPoint:subviewFrame.origin];
}
/// 根据当前view 找所在collectionview 里的 indexpath
NS_INLINE NSIndexPath * kIndexpathSubviewCollectionview(UIView *subview, UICollectionView *collectionview){
    CGRect subviewFrame = [subview convertRect:subview.bounds toView:collectionview];
    return [collectionview indexPathForItemAtPoint:subviewFrame.origin];
}
/// 根据当前view 找所在tableview 里的 tableviewcell
NS_INLINE UITableViewCell * kCellSubviewTableview(UIView *subview, UITableView *tableview){
    CGRect subviewFrame = [subview convertRect:subview.bounds toView:tableview];
    NSIndexPath *indexPath = [tableview indexPathForRowAtPoint:subviewFrame.origin];
    return [tableview cellForRowAtIndexPath:indexPath];
}
#pragma mark -------------- GCD 线程处理 -------------
/// 主线程
NS_INLINE void kGCD_main(dispatch_block_t block) {
    if ([[NSThread currentThread] isMainThread]) {
        dispatch_async(dispatch_get_main_queue(), block);
    }else {
        dispatch_sync(dispatch_get_main_queue(), block);
    }
}
/// 子线程
NS_INLINE void kGCD_async(dispatch_block_t block) {
    dispatch_queue_t queue = dispatch_queue_create("com.yangkejun.gcd", DISPATCH_QUEUE_CONCURRENT);
    dispatch_async(queue, block);
}
/// 线程异步并行队列
NS_INLINE void kGCD_group_notify(dispatch_block_t block,dispatch_block_t notify) {
    dispatch_queue_t queue = dispatch_queue_create("com.yangkejun.gcd", DISPATCH_QUEUE_CONCURRENT);
    dispatch_group_t group = dispatch_group_create();
    dispatch_group_async(group, queue, block);
    dispatch_group_notify(group, queue, ^{ dispatch_async(dispatch_get_main_queue(), notify); });
}
/// 延时执行
NS_INLINE void kGCD_after(int64_t delayInSeconds, dispatch_block_t block) {
    dispatch_queue_t queue = dispatch_queue_create("com.yangkejun.gcd", DISPATCH_QUEUE_CONCURRENT);
    dispatch_time_t time = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delayInSeconds * NSEC_PER_SEC));
    dispatch_after(time, queue, block);
}
/// 栅栏
NS_INLINE dispatch_queue_t kGCD_barrier(dispatch_block_t block, dispatch_block_t barrier) {
    dispatch_queue_t queue = dispatch_queue_create("com.yangkejun.gcd", DISPATCH_QUEUE_CONCURRENT);
    dispatch_async(queue, block);
    dispatch_barrier_async(queue, barrier);
    return queue;
}
/// 一次性
NS_INLINE void kGCD_once(dispatch_block_t block) {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, block);
}
/// 快速迭代
NS_INLINE void kGCD_apply(int iterations, void (^block)(size_t)) {
    dispatch_queue_t queue = dispatch_queue_create("com.yangkejun.gcd", DISPATCH_QUEUE_CONCURRENT);
    dispatch_apply(iterations, queue, block);
}

#pragma clang diagnostic pop
NS_ASSUME_NONNULL_END
#endif

#endif /* _KJINLINE_h */
