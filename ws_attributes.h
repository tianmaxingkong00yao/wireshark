/* ws_attributes.h
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef __WS_ATTRIBUTES_H__
#define __WS_ATTRIBUTES_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * If we're running GCC or clang define _U_ to be "__attribute__((unused))"
 * so we can use _U_ to flag unused function parameters and not get warnings
 * about them. Otherwise, define _U_ to be an empty string so that _U_ used
 * to flag an unused function parameters will compile with other compilers.
 *
 * XXX - similar hints for other compilers?
 */

/*
 * This was introduced by Clang:
 *
 *     http://clang.llvm.org/docs/LanguageExtensions.html#has-attribute
 *
 * in some version (which version?); it has been picked up by GCC 5.0.
 */
#ifndef __has_attribute
  /*
   * It's a macro, so you can check whether it's defined to check
   * whether it's supported.
   *
   * If it's not, define it to always return 0, so that we move on to
   * the fallback checks.
   */
  #define __has_attribute(x) 0
#endif

/*
 * Note that the C90 spec's "6.8.1 Conditional inclusion" and the
 * C99 spec's and C11 spec's "6.10.1 Conditional inclusion" say:
 *
 *    Prior to evaluation, macro invocations in the list of preprocessing
 *    tokens that will become the controlling constant expression are
 *    replaced (except for those macro names modified by the defined unary
 *    operator), just as in normal text.  If the token "defined" is
 *    generated as a result of this replacement process or use of the
 *    "defined" unary operator does not match one of the two specified
 *    forms prior to macro replacement, the behavior is undefined.
 *
 * so you shouldn't use defined() in a #define that's used in #if or
 * #elif.  Some versions of Clang, for example, will warn about this.
 *
 * Instead, we check whether the pre-defined macros for particular
 * compilers are defined and, if not, define the "is this version XXX
 * or a later version of this compiler" macros as 0.
 */

/*
 * Check whether this is GCC major.minor or a later release, or some
 * compiler that claims to be "just like GCC" of that version or a
 * later release.
 */

#if !defined(__GNUC__)
  #define WS_IS_AT_LEAST_GNUC_VERSION(major, minor) 0
#else
  #define WS_IS_AT_LEAST_GNUC_VERSION(major, minor) \
	(__GNUC__ > (major) || \
	 (__GNUC__ == (major) && __GNUC_MINOR__ >= (minor)))
#endif

/*
 * Check whether this is Sun C/SunPro C/Oracle Studio major.minor
 * or a later release.
 *
 * The version number in __SUNPRO_C is encoded in hex BCD, with the
 * uppermost hex digit being the major version number, the next
 * one or two hex digits being the minor version number, and
 * the last digit being the patch version.
 *
 * It represents the *compiler* version, not the product version;
 * see
 *
 *    https://sourceforge.net/p/predef/wiki/Compilers/
 *
 * for a partial mapping, which we assume continues for later
 * 12.x product releases.
 */

#if !defined(__SUNPRO_C)
  #define WS_IS_AT_LEAST_SUNC_VERSION(major,minor) 0
#else
  #define WS_SUNPRO_VERSION_TO_BCD(major, minor) \
	(((minor) >= 10) ? \
	    (((major) << 12) | (((minor)/10) << 8) | (((minor)%10) << 4)) : \
	    (((major) << 8) | ((minor) << 4)))
  #define WS_IS_AT_LEAST_SUNC_VERSION(major,minor) \
	(__SUNPRO_C >= WS_SUNPRO_VERSION_TO_BCD((major), (minor)))
#endif

/*
 * Check whether this is IBM XL C major.minor or a later release.
 *
 * The version number in __xlC__ has the major version in the
 * upper 8 bits and the minor version in the lower 8 bits.
 */

#if !defined(__xlC__)
  #define WS_IS_AT_LEAST_XL_C_VERSION(major,minor) 0
#else
  #define WS_IS_AT_LEAST_XL_C_VERSION(major, minor) \
	(__xlC__ >= (((major) << 8) | (minor)))
#endif

/*
 * Check whether this is HP aC++/HP C major.minor or a later release.
 *
 * The version number in __HP_aCC is encoded in zero-padded decimal BCD,
 * with the "A." stripped off, the uppermost two decimal digits being
 * the major version number, the next two decimal digits being the minor
 * version number, and the last two decimal digits being the patch version.
 * (Strip off the A., remove the . between the major and minor version
 * number, and add two digits of patch.)
 */

#if !defined(__HP_aCC)
  #define WS_IS_AT_LEAST_HP_C_VERSION(major,minor) 0
#else
  #define WS_IS_AT_LEAST_HP_C_VERSION(major,minor) \
	(__HP_aCC >= ((major)*10000 + (minor)*100))
#endif

#if defined(__GNUC__)
  /* This includes clang */
  #define _U_ __attribute__((unused))
#else
  #define _U_
#endif

/*
 * WS_NORETURN, before a function declaration, means "this function
 * never returns".  (It must go before the function declaration, e.g.
 * "extern WS_NORETURN func(...)" rather than after the function
 * declaration, as the MSVC version has to go before the declaration.)
 */
#if __has_attribute(noreturn) \
    || WS_IS_AT_LEAST_GNUC_VERSION(2,5) \
    || WS_IS_AT_LEAST_SUNC_VERSION(5,9) \
    || WS_IS_AT_LEAST_XL_C_VERSION(10,1) \
    || WS_IS_AT_LEAST_HP_C_VERSION(6,10)
  /*
   * Compiler with support for __attribute__((noreturn)), or GCC 2.5 and
   * later, or Solaris Studio 12 (Sun C 5.9) and later, or IBM XL C 10.1
   * and later (do any earlier versions of XL C support this?), or
   * HP aCC A.06.10 and later.
   */
  #define WS_NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
  /*
   * MSVC.
   */
  #define WS_NORETURN __declspec(noreturn)
#else
  #define WS_NORETURN
#endif

/* Hint to the compiler that the function returns a non-null value */
#if __has_attribute(returns_nonnull) \
    || WS_IS_AT_LEAST_GNUC_VERSION(4,9)
  #define WS_RETNONNULL __attribute__((returns_nonnull))
#else
  #define WS_RETNONNULL
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __WS_ATTRIBUTES_H__ */