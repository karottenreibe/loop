; ModuleID = 'header.c'
target datalayout = "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:32:64-f32:32:32-f64:32:64-v64:64:64-v128:128:128-a0:0:64-f80:32:32-n8:16:32"
target triple = "i386-pc-linux-gnu"

@.str = private constant [119 x i8] c"This is a LOOP program.\0AIt expects a single argument: a positive integer, which will be assigned to the variable `n'.\0A\00" ; <[119 x i8]*> [#uses=1]
@.str1 = private constant [35 x i8] c"Program for n=%i evaluated to: %i\0A\00" ; <[35 x i8]*> [#uses=1]

define i32 @main(i32 %argc, i8** %argv) nounwind {
  %1 = alloca i32, align 4                        ; <i32*> [#uses=4]
  %2 = alloca i32, align 4                        ; <i32*> [#uses=2]
  %3 = alloca i8**, align 4                       ; <i8***> [#uses=2]
  %arg = alloca i32, align 4                      ; <i32*> [#uses=3]
  %ret = alloca i32, align 4                      ; <i32*> [#uses=2]
  store i32 0, i32* %1
  store i32 %argc, i32* %2
  store i8** %argv, i8*** %3
  %4 = load i32* %2                               ; <i32> [#uses=1]
  %5 = icmp ne i32 %4, 2                          ; <i1> [#uses=1]
  br i1 %5, label %6, label %8

; <label>:6                                       ; preds = %0
  %7 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([119 x i8]* @.str, i32 0, i32 0)) ; <i32> [#uses=0]
  store i32 1, i32* %1
  br label %18

; <label>:8                                       ; preds = %0
  %9 = load i8*** %3                              ; <i8**> [#uses=1]
  %10 = getelementptr inbounds i8** %9, i32 1     ; <i8**> [#uses=1]
  %11 = load i8** %10                             ; <i8*> [#uses=1]
  %12 = call i32 @atoi(i8* %11) nounwind readonly ; <i32> [#uses=1]
  store i32 %12, i32* %arg
  %13 = load i32* %arg                            ; <i32> [#uses=1]
  %14 = call i32 @mainloop(i32 %13)               ; <i32> [#uses=1]
  store i32 %14, i32* %ret
  %15 = load i32* %arg                            ; <i32> [#uses=1]
  %16 = load i32* %ret                            ; <i32> [#uses=1]
  %17 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([35 x i8]* @.str1, i32 0, i32 0), i32 %15, i32 %16) ; <i32> [#uses=0]
  store i32 0, i32* %1
  br label %18

; <label>:18                                      ; preds = %8, %6
  %19 = load i32* %1                              ; <i32> [#uses=1]
  ret i32 %19
}

declare i32 @printf(i8*, ...)

declare i32 @atoi(i8*) nounwind readonly

declare i32 @mainloop(i32)
