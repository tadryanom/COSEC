#ifndef __COSEC_TYPES_H__
#define __COSEC_TYPES_H__

/*
 *  primitive types
 */

#define NOERR       0

typedef  unsigned int  size_t;
typedef  int           ptrdiff_t;
typedef  int           off_t;

typedef  unsigned long long int  dev_t;

static inline unsigned int
gnu_dev_major (dev_t __dev)
{
      return ((__dev >> 8) & 0xfff) | ((unsigned int) (__dev >> 32) & ~0xfff);
}

static inline unsigned int
gnu_dev_minor (dev_t __dev)
{
      return (__dev & 0xff) | ((unsigned int) (__dev >> 12) & ~0xff);
}

static inline dev_t
gnu_dev_makedev (unsigned int __major, unsigned int __minor)
{
      return ((__minor & 0xff) | ((__major & 0xfff) << 8)
                        | (((dev_t) (__minor & ~0xff)) << 12)
                                  | (((dev_t) (__major & ~0xfff)) << 32));
}

/* Access the functions with their traditional names.  */
# define major(dev) gnu_dev_major (dev)
# define minor(dev) gnu_dev_minor (dev)
# define makedev(maj, min) gnu_dev_makedev (maj, min)

#endif //__COSEC_TYPES_H__