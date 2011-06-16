/*
 * celix_errno.h
 *
 *  Created on: Feb 15, 2011
 *      Author: alexanderb
 */
/*!
  \file
  \brief Error codes
  \defgroup framework Celix Framework
 */
#ifndef CELIX_ERRNO_H_
#define CELIX_ERRNO_H_

#include <errno.h>

/*!
 * \defgroup celix_errno Error Codes
 * \ingroup framework
 * \{
 */

/*!
 * Status type returned by all functions in Celix
 */
typedef int celix_status_t;

/*!
 * Error code indicating successful execution of the function.
 */
#define CELIX_SUCCESS 0

/*!
 * Starting point for Celix errors.
 */
#define CELIX_START_ERROR 20000

/*!
 * Exception indicating a problem with a bundle
 */
#define CELIX_BUNDLE_EXCEPTION (CELIX_START_ERROR + 1)
/*!
 * Invalid bundle context is used
 */
#define CELIX_INVALID_BUNDLE_CONTEXT (CELIX_START_ERROR + 2)
/*!
 * Argument is not correct
 */
#define CELIX_ILLEGAL_ARGUMENT (CELIX_START_ERROR + 3)
#define CELIX_INVALID_SYNTAX (CELIX_START_ERROR + 4)
#define CELIX_FRAMEWORK_SHUTDOWN (CELIX_START_ERROR + 5)
#define CELIX_ILLEGAL_STATE (CELIX_START_ERROR + 6)
#define CELIX_FRAMEWORK_EXCEPTION (CELIX_START_ERROR + 7)
#define CELIX_FILE_IO_EXCEPTION (CELIX_START_ERROR + 8)

#define CELIX_ENOMEM ENOMEM

/**
 * \}
 */

#endif /* CELIX_ERRNO_H_ */