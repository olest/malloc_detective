// for RTLD_NEXT
#define _GNU_SOURCE 

#include <stdio.h>
#include <limits.h>
#include <dlfcn.h>
#include <malloc.h>

static void* (*real_malloc)(size_t)=NULL;

// https://www.gnu.org/software/libc/manual/html_node/Hooks-for-Malloc.html
// https://developers.redhat.com/articles/2021/08/25/securing-malloc-glibc-why-malloc-hooks-had-go
// https://stackoverflow.com/questions/6083337/overriding-malloc-using-the-ld-preload-mechanism
// https://www.man7.org/linux/man-pages/man3/mallinfo.3.html
// https://stackoverflow.com/questions/1696671/how-to-detect-programmatically-count-of-bytes-allocated-by-process-on-heap
// https://www.gnu.org/software/libc/manual/html_node/Statistics-of-Malloc.html
// https://www.gnu.org/software/libc/manual/html_node/Unconstrained-Allocation.html

void __attribute__((constructor)) at_launch() 
{
    struct mallinfo2 info = mallinfo2();
    
    printf("At launch:");
    printf("    total non-mmapped bytes  (arena): %zu\n", info.arena);
    printf("    total allocated bytes (wordblks): %lu\n", info.uordblks);
    printf("    total free bytes      (fordblks): %lu\n", info.fordblks);
}

void __attribute__((destructor)) at_end() 
{
    struct mallinfo2 info = mallinfo2();
    
    printf("Summary:");
    printf("    total non-mmapped bytes  (arena): %zu\n", info.arena);
    printf("    total allocated bytes (wordblks): %lu\n", info.uordblks);
    printf("    total free bytes      (fordblks): %lu\n", info.fordblks);
}

static void init_detective(void)
{
    real_malloc = dlsym(RTLD_NEXT,"malloc");
    if (real_malloc == NULL)
        fprintf(stderr,"Error in dlsym: %s\n",dlerror());

}

void *malloc(size_t size)
{
    if(real_malloc==NULL) {
        init_detective();
    }

    void *p = NULL;
    fprintf(stderr, "malloc(%lu) = ", size);
    p = real_malloc(size);
    fprintf(stderr, "%p\n", p);
    return p;
}
