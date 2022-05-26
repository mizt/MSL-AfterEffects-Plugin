namespace FileManager {
    
    bool exists(NSString *path) {
        static const NSFileManager *fileManager = [NSFileManager defaultManager];
        NSError *err = nil;
        [fileManager attributesOfItemAtPath:path error:&err];
        return (err)?false:true;
    }
    
    bool extension(NSString *filename, NSString *ext) {
        return ([[filename pathExtension] compare:ext]==NSOrderedSame)?true:false;
    }

    long size(NSString *filename) {
        NSError *err = nil;
        NSDictionary *attributes = [[NSFileManager defaultManager] attributesOfItemAtPath:filename error:&err];
        return (err)?-1:[[attributes objectForKey:NSFileSize] integerValue];
    }

    int indexOf(NSString *path, NSString *search) {
        NSRange range = [path rangeOfString:search];
        return (range.location==NSNotFound)?-1:(int)range.location;
    }

    NSString *replace(NSString *filename, NSString *from, NSString *to) {
        return [filename stringByReplacingOccurrencesOfString:from withString:to];
    }
    
    NSString *replace(NSString *filename, NSArray<NSString *> *from, NSString *to) {
        NSMutableString *mstr = [NSMutableString stringWithString:filename];
        for(int n=0; n<[from count]; n++) {
            mstr.string = replace(mstr,from[n],to);
        }
        return [NSString stringWithString:mstr];
    }

    bool eq(NSString *a, NSString *b) {
        return ([a compare:b]==NSOrderedSame)?true:false;
    }
    
    NSString *concat(NSString *a, NSString *b) {
        return (a==nil)?b:replace([NSString stringWithFormat:@"%@/%@",a,b],@"//",@"/");
    }
    
    NSString *removePlatform(NSString *str) {
        NSString *extension = [NSString stringWithFormat:@".%@",[str pathExtension]];
        return FileManager::replace(str,@[
            [NSString stringWithFormat:@"-macosx%@",extension],
            [NSString stringWithFormat:@"-iphoneos%@",extension],
            [NSString stringWithFormat:@"-iphonesimulator%@",extension]],
            extension);
    }
    
    NSString *addPlatform(NSString *str) {
        NSString *extension = [NSString stringWithFormat:@".%@",[str pathExtension]];
#if TARGET_OS_OSX
        return FileManager::replace(FileManager::removePlatform(str),extension,[NSString stringWithFormat:@"-macosx%@",extension]);
#elif TARGET_OS_SIMULATOR
        return FileManager::replace(FileManager::removePlatform(str),extension,[NSString stringWithFormat:@"-iphonesimulator%@",extension]);
#elif TARGET_OS_IPHONE
        return FileManager::replace(FileManager::removePlatform(str),extension,[NSString stringWithFormat:@"-iphoneos%@",extension]);
#else
        return nil;
#endif
    }
    
    NSString *path(NSString *filename,NSString *identifier=nil) {
        if(identifier==nil) {
#if TARGET_OS_OSX
            return [NSString stringWithFormat:@"%@/%@",[[NSBundle mainBundle] bundlePath],filename];
#else
            return [NSString stringWithFormat:@"%@/%@",[[NSBundle mainBundle] resourcePath],filename];
#endif
        }
        else {
            return [NSString stringWithFormat:@"%@/%@",[[NSBundle bundleWithIdentifier:identifier] resourcePath],filename];
        }
    }

    NSString *resource(NSString *identifier, NSString *filename, NSString *ext) {
        return [[[NSBundle bundleWithIdentifier:identifier] URLForResource:filename withExtension:ext] path];
    }

    NSString *resource(NSString *identifier, NSString *filename) {
        return resource(identifier,[filename stringByDeletingPathExtension],[filename pathExtension]);
    }
};
