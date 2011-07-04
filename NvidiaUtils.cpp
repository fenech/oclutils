/**
 * Comes from oclUtils.{cpp,h} and shrUtils.{cpp,h} from Nvidia's CUDA SDK
 */


#include "NvidiaUtils.hpp"

// Defines for GPU Architecture types (using the SM version to determine the # of cores per SM
static int nGpuArchCoresPerSM[] = { -1, 8, 32 };
// end of GPU Architecture definitions

//////////////////////////////////////////////////////////////////////////////
//! Gets the platform ID for NVIDIA if available, otherwise default
//!
//! @return the id
//! @param clSelectedPlatformID         OpenCL platoform ID
//////////////////////////////////////////////////////////////////////////////
cl_int oclGetPlatformID(cl_platform_id* clSelectedPlatformID)
{
    char chBuffer[1024];
    cl_uint num_platforms;
    cl_platform_id* clPlatformIDs;
    cl_int ciErrNum;
    *clSelectedPlatformID = NULL;

    // Get OpenCL platform count
    ciErrNum = clGetPlatformIDs (0, NULL, &num_platforms);
    if (ciErrNum != CL_SUCCESS)
    {
        if (ciErrNum == CL_INVALID_VALUE)
            std_cout << "CL_INVALID_VALUE error when calling clGetPlatformIDs() in NvidiaUtils.cpp line 27\n";
        //else if (ciErrNum == CL_PLATFORM_NOT_FOUND_KHR)
        //    std_cout << "CL_PLATFORM_NOT_FOUND_KHR error when calling clGetPlatformIDs() in NvidiaUtils.cpp line 27\n";
        else
            std_cout << " Error " << (int) ciErrNum << " in clGetPlatformIDs Call !!!\n";
        return -1000;
    }
    else
    {
        if(num_platforms == 0)
        {
            std_cout << "No OpenCL platform found!\n";
            return -2000;
        }
        else
        {
            // if there's a platform or more, make space for ID's
            if ((clPlatformIDs = (cl_platform_id*)malloc(num_platforms * sizeof(cl_platform_id))) == NULL)
            {
                std_cout << "Failed to allocate memory for cl_platform ID's!\n";
                return -3000;
            }

            // get platform info for each platform and trap the NVIDIA platform if found
            ciErrNum = clGetPlatformIDs (num_platforms, clPlatformIDs, NULL);
            for(cl_uint i = 0; i < num_platforms; ++i)
            {
                ciErrNum = clGetPlatformInfo (clPlatformIDs[i], CL_PLATFORM_NAME, 1024, &chBuffer, NULL);
                if(ciErrNum == CL_SUCCESS)
                {
                    if(strstr(chBuffer, "NVIDIA") != NULL)
                    {
                        *clSelectedPlatformID = clPlatformIDs[i];
                        break;
                    }
                }
            }

            // default to zeroeth platform if NVIDIA not found
            if(*clSelectedPlatformID == NULL)
            {
                std_cout << "WARNING: NVIDIA OpenCL platform not found - defaulting to first platform!\n";
                *clSelectedPlatformID = clPlatformIDs[0];
            }

            free(clPlatformIDs);
        }
    }

    return CL_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////
//! Print the device name
//!
//! @param iLogMode       enum LOGBOTH, LOGCONSOLE, LOGFILE
//! @param device         OpenCL id of the device
//////////////////////////////////////////////////////////////////////////////
void oclPrintDevName(cl_device_id device)
{
    char device_string[1024];
    clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(device_string), &device_string, NULL);
    std_cout << "Device name: " << device_string << "\n";
}

//////////////////////////////////////////////////////////////////////////////
//! Get and return device capability
//!
//! @return the 2 digit integer representation of device Cap (major minor). return -1 if NA
//! @param device         OpenCL id of the device
//////////////////////////////////////////////////////////////////////////////
int oclGetDevCap(cl_device_id device)
{
    char cDevString[1024];
    bool bDevAttributeQuery = false;
    int iDevArch = -1;

    // Get device extensions, and if any then search for cl_nv_device_attribute_query
    clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, sizeof(cDevString), &cDevString, NULL);
    if (cDevString != 0)
    {
        std::string stdDevString;
        stdDevString = std::string(cDevString);
        size_t szOldPos = 0;
        size_t szSpacePos = stdDevString.find(' ', szOldPos); // extensions string is space delimited
        while (szSpacePos != stdDevString.npos)
        {
            if( strcmp("cl_nv_device_attribute_query", stdDevString.substr(szOldPos, szSpacePos - szOldPos).c_str()) == 0 )
            {
                bDevAttributeQuery = true;
            }

            do {
                szOldPos = szSpacePos + 1;
                szSpacePos = stdDevString.find(' ', szOldPos);
            } while (szSpacePos == szOldPos);
        }
    }

    // if search succeeded, get device caps
    if(bDevAttributeQuery)
    {
        cl_int iComputeCapMajor, iComputeCapMinor;
        clGetDeviceInfo(device, CL_DEVICE_COMPUTE_CAPABILITY_MAJOR_NV, sizeof(cl_uint), (void*)&iComputeCapMajor, NULL);
        clGetDeviceInfo(device, CL_DEVICE_COMPUTE_CAPABILITY_MINOR_NV, sizeof(cl_uint), (void*)&iComputeCapMinor, NULL);
        iDevArch = (10 * iComputeCapMajor) + iComputeCapMinor;
    }

    return iDevArch;
}

// Helper function to get OpenCL error string from constant
// *********************************************************************
const char* oclErrorString(cl_int error)
{
    static const char* errorString[] = {
        "CL_SUCCESS",
        "CL_DEVICE_NOT_FOUND",
        "CL_DEVICE_NOT_AVAILABLE",
        "CL_COMPILER_NOT_AVAILABLE",
        "CL_MEM_OBJECT_ALLOCATION_FAILURE",
        "CL_OUT_OF_RESOURCES",
        "CL_OUT_OF_HOST_MEMORY",
        "CL_PROFILING_INFO_NOT_AVAILABLE",
        "CL_MEM_COPY_OVERLAP",
        "CL_IMAGE_FORMAT_MISMATCH",
        "CL_IMAGE_FORMAT_NOT_SUPPORTED",
        "CL_BUILD_PROGRAM_FAILURE",
        "CL_MAP_FAILURE",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "CL_INVALID_VALUE",
        "CL_INVALID_DEVICE_TYPE",
        "CL_INVALID_PLATFORM",
        "CL_INVALID_DEVICE",
        "CL_INVALID_CONTEXT",
        "CL_INVALID_QUEUE_PROPERTIES",
        "CL_INVALID_COMMAND_QUEUE",
        "CL_INVALID_HOST_PTR",
        "CL_INVALID_MEM_OBJECT",
        "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
        "CL_INVALID_IMAGE_SIZE",
        "CL_INVALID_SAMPLER",
        "CL_INVALID_BINARY",
        "CL_INVALID_BUILD_OPTIONS",
        "CL_INVALID_PROGRAM",
        "CL_INVALID_PROGRAM_EXECUTABLE",
        "CL_INVALID_KERNEL_NAME",
        "CL_INVALID_KERNEL_DEFINITION",
        "CL_INVALID_KERNEL",
        "CL_INVALID_ARG_INDEX",
        "CL_INVALID_ARG_VALUE",
        "CL_INVALID_ARG_SIZE",
        "CL_INVALID_KERNEL_ARGS",
        "CL_INVALID_WORK_DIMENSION",
        "CL_INVALID_WORK_GROUP_SIZE",
        "CL_INVALID_WORK_ITEM_SIZE",
        "CL_INVALID_GLOBAL_OFFSET",
        "CL_INVALID_EVENT_WAIT_LIST",
        "CL_INVALID_EVENT",
        "CL_INVALID_OPERATION",
        "CL_INVALID_GL_OBJECT",
        "CL_INVALID_BUFFER_SIZE",
        "CL_INVALID_MIP_LEVEL",
        "CL_INVALID_GLOBAL_WORK_SIZE",
    };

    const int errorCount = sizeof(errorString) / sizeof(errorString[0]);

    const int index = -error;

    return (index >= 0 && index < errorCount) ? errorString[index] : "Unspecified Error";
}

// *********************************************************************
// Function to log standardized information to console, file or both
static int shrLogV(int iLogMode, int iErrNum, const char* cFormatString, va_list vaArgList)
{
    /*
    static FILE* pFileStream0 = NULL;
    static FILE* pFileStream1 = NULL;
    size_t szNumWritten = 0;
    char cFileMode [3];

    // if the sample log file is closed and the call incudes a "write-to-file", open file for writing
    if ((pFileStream0 == NULL) && (iLogMode & LOGFILE))
    {
        // if the default filename has not been overriden, set to default
        if (cLogFilePathAndName == NULL)
        {
            shrSetLogFileName(DEFAULTLOGFILE);
        }

        #ifdef _WIN32   // Windows version
            // set the file mode
            if (iLogMode & APPENDMODE)  // append to prexisting file contents
            {
                sprintf_s (cFileMode, 3, "a+");
            }
            else                        // replace prexisting file contents
            {
                sprintf_s (cFileMode, 3, "w");
            }

            // open the individual sample log file in the requested mode
            errno_t err = fopen_s(&pFileStream0, cLogFilePathAndName, cFileMode);

            // if error on attempt to open, be sure the file is null or close it, then return negative error code
            if (err != 0)
            {
                if (pFileStream0)
                {
                    fclose (pFileStream0);
                }
                return -err;
            }
        #else           // Linux & Mac version
            // set the file mode
            if (iLogMode & APPENDMODE)  // append to prexisting file contents
            {
                sprintf (cFileMode, "a+");
            }
            else                        // replace prexisting file contents
            {
                sprintf (cFileMode, "w");
            }

            // open the file in the requested mode
            if ((pFileStream0 = fopen(cLogFilePathAndName, cFileMode)) == 0)
            {
                // if error on attempt to open, be sure the file is null or close it, then return negative error code
                if (pFileStream0)
                {
                    fclose (pFileStream0);
                }
                return -1;
            }
        #endif
    }
    */

    /*
    // if the master log file is closed and the call incudes a "write-to-file" and MASTER, open master logfile file for writing
    if ((pFileStream1 == NULL) && (iLogMode & LOGFILE) && (iLogMode & MASTER))
    {
        #ifdef _WIN32   // Windows version
            // open the master log file in append mode
            errno_t err = fopen_s(&pFileStream1, MASTERLOGFILE, "a+");

            // if error on attempt to open, be sure the file is null or close it, then return negative error code
            if (err != 0)
            {
                if (pFileStream1)
                {
                    fclose (pFileStream1);
                }
                return -err;
            }
        #else           // Linux & Mac version

            // open the file in the requested mode
            if ((pFileStream1 = fopen(MASTERLOGFILE, "a+")) == 0)
            {
                // if error on attempt to open, be sure the file is null or close it, then return negative error code
                if (pFileStream1)
                {
                    fclose (pFileStream1);
                }
                return -1;
            }
        #endif

        // If master log file length has become excessive, empty/reopen
        fseek(pFileStream1, 0L, SEEK_END);
        if (ftell(pFileStream1) > 50000L)
        {
            fclose (pFileStream1);
        #ifdef _WIN32   // Windows version
            fopen_s(&pFileStream1, MASTERLOGFILE, "w");
        #else
            pFileStream1 = fopen(MASTERLOGFILE, "w");
        #endif
        }
    }
    */

    /*
    // Handle special Error Message code
    if (iLogMode & ERRORMSG)
    {
        // print string to console if flagged
        if (iLogMode & LOGCONSOLE)
        {
            szNumWritten = printf ("\n !!! Error # %i at ", iErrNum);                           // console
        }
        // print string to file if flagged
        if (iLogMode & LOGFILE)
        {
            szNumWritten = fprintf (pFileStream0, "\n !!! Error # %i at ", iErrNum);            // sample log file
        }
    }
    */

    // Vars used for variable argument processing
    const char*     pStr;
    const char*     cArg;
    int             iArg;
    double          dArg;
    unsigned int    uiArg;
    std::string sFormatSpec;
    const std::string sFormatChars = " -+#0123456789.dioufnpcsXxEeGgAa";
    const std::string sTypeChars = "dioufnpcsXxEeGgAa";
    char cType = 'c';

    // Start at the head of the string and scan to the null at the end
    for (pStr = cFormatString; *pStr; ++pStr)
    {
        // Check if the current character is not a formatting specifier ('%')
        if (*pStr != '%')
        {
            /*
            // character is not '%', so print it verbatim to console and/or files as flagged
            if (iLogMode & LOGCONSOLE)
            {
                szNumWritten = putc(*pStr, stdout);                                             // console
            }
            if (iLogMode & LOGFILE)
            {
                szNumWritten  = putc(*pStr, pFileStream0);                                      // sample log file
                if (iLogMode & MASTER)
                {
                    szNumWritten = putc(*pStr, pFileStream1);                                   // master log file
                }
            }
            */
            std_cout << *pStr;
        }
        else
        {
            // character is '%', so skip over it and read the full format specifier for the argument
            ++pStr;
            sFormatSpec = '%';

            // special handling for string of %%%%
            bool bRepeater = (*pStr == '%');
            if (bRepeater)
            {
                cType = '%';
            }

            // chars after the '%' are part of format if on list of constants... scan until that isn't true or NULL is found
            while (pStr && ((sFormatChars.find(*pStr) != std::string::npos) || bRepeater))
            {
                sFormatSpec += *pStr;

                // If the char is a type specifier, trap it and stop scanning
                // (a type specifier char is always the last in the format except for string of %%%)
                if (sTypeChars.find(*pStr) != std::string::npos)
                {
                    cType = *pStr;
                    break;
                }

                // Special handling for string of %%%
                // If a string of %%% was started and then it ends, break (There won't be a typical type specifier)
                if (bRepeater && (*pStr != '%'))
                {
                    break;
                }

                pStr++;
            }

            // Now handle the arg according to type
            switch (cType)
            {
                case '%':   // special handling for string of %%%%
                {
                    if (iLogMode & LOGCONSOLE)
                    {
                        //szNumWritten = printf(sFormatSpec.c_str());                             // console
                        std_cout << sFormatSpec;
                    }
                    /*
                    if (iLogMode & LOGFILE)
                    {
                        szNumWritten = fprintf (pFileStream0, sFormatSpec.c_str());             // sample log file
                        if (iLogMode & MASTER)
                        {
                            szNumWritten  = fprintf(pFileStream1, sFormatSpec.c_str());         // master log file
                        }
                    }
                    */
                    continue;
                }
                case 'c':   // single byte char
                case 's':   // string of single byte chars
                {
                    // Set cArg as the next value in list and print to console and/or files if flagged
                    cArg = va_arg(vaArgList, char*);
                    if (iLogMode & LOGCONSOLE)
                    {
                        //szNumWritten = printf(sFormatSpec.c_str(), cArg);                       // console
                        std_cout << cArg;
                    }
                    /*
                    if (iLogMode & LOGFILE)
                    {
                        szNumWritten = fprintf (pFileStream0, sFormatSpec.c_str(), cArg);       // sample log file
                        if (iLogMode & MASTER)
                        {
                            szNumWritten  = fprintf(pFileStream1, sFormatSpec.c_str(), cArg);   // master log file
                        }
                    }
                    */
                    continue;
                }
                case 'd':   // signed decimal integer
                case 'i':   // signed decimal integer
                {
                    // set iArg as the next value in list and print to console and/or files if flagged
                    iArg = va_arg(vaArgList, int);
                    if (iLogMode & LOGCONSOLE)
                    {
                        //szNumWritten = printf(sFormatSpec.c_str(), iArg);                       // console
                        std_cout << iArg;
                    }
                    /*
                    if (iLogMode & LOGFILE)
                    {
                        szNumWritten = fprintf (pFileStream0, sFormatSpec.c_str(), iArg);       // sample log file
                        if (iLogMode & MASTER)
                        {
                            szNumWritten  = fprintf(pFileStream1, sFormatSpec.c_str(), iArg);   // master log file
                        }
                    }
                    */
                    continue;
                }
                case 'u':   // unsigned decimal integer
                case 'o':   // unsigned octal integer
                case 'x':   // unsigned hexadecimal integer using "abcdef"
                case 'X':   // unsigned hexadecimal integer using "ABCDEF"
                {
                    // set uiArg as the next value in list and print to console and/or files if flagged
                    uiArg = va_arg(vaArgList, unsigned int);
                    if (iLogMode & LOGCONSOLE)
                    {
                        //szNumWritten = printf(sFormatSpec.c_str(), uiArg);                      // console
                        std_cout << uiArg;
                    }
                    /*
                    if (iLogMode & LOGFILE)
                    {
                        szNumWritten = fprintf (pFileStream0, sFormatSpec.c_str(), uiArg);      // sample log file
                        if (iLogMode & MASTER)
                        {
                            szNumWritten  = fprintf(pFileStream1, sFormatSpec.c_str(), uiArg);  // master log file
                        }
                    }
                    */
                    continue;
                }
                case 'f':   // float/double
                case 'e':   // scientific double/float
                case 'E':   // scientific double/float
                case 'g':   // scientific double/float
                case 'G':   // scientific double/float
                case 'a':   // signed hexadecimal double precision float
                case 'A':   // signed hexadecimal double precision float
                {
                    // set dArg as the next value in list and print to console and/or files if flagged
                    dArg = va_arg(vaArgList, double);
                    if (iLogMode & LOGCONSOLE)
                    {
                        //szNumWritten = printf(sFormatSpec.c_str(), dArg);                       // console
                        std_cout << dArg;
                    }
                    /*
                    if (iLogMode & LOGFILE)
                    {
                        szNumWritten = fprintf (pFileStream0, sFormatSpec.c_str(), dArg);       // sample log file
                        if (iLogMode & MASTER)
                        {
                            szNumWritten  = fprintf(pFileStream1, sFormatSpec.c_str(), dArg);   // master log file
                        }
                    }
                    */
                    continue;
                }
                default:
                {
                    // print arg of unknown/unsupported type to console and/or file if flagged
                    if (iLogMode & LOGCONSOLE)                          // console
                    {
                        //szNumWritten = putc(*pStr, stdout);
                        std_cout << *pStr;
                    }
                    /*
                    if (iLogMode & LOGFILE)
                    {
                        szNumWritten  = putc(*pStr, pFileStream0);      // sample log file
                        if (iLogMode & MASTER)
                        {
                            szNumWritten  = putc(*pStr, pFileStream1);  // master log file
                        }
                    }
                    */
                }
            }
        }
    }

    /*
    // end the sample log with a horizontal line if closing
    if (iLogMode & CLOSELOG)
    {
        if (iLogMode & LOGCONSOLE)
        {
            printf(HDASHLINE);
        }
        if (iLogMode & LOGFILE)
        {
            fprintf(pFileStream0, HDASHLINE);
        }
    }
    */

    // flush console and/or file buffers if updated
    if (iLogMode & LOGCONSOLE)
    {
        //fflush(stdout);
        std_cout.Flush();
    }
    /*
    if (iLogMode & LOGFILE)
    {
        fflush (pFileStream0);

        // if the master log file has been updated, flush it too
        if (iLogMode & MASTER)
        {
            fflush (pFileStream1);
        }
    }
    */

    /*
    // If the log file is open and the caller requests "close file", then close and NULL file handle
    if ((pFileStream0) && (iLogMode & CLOSELOG))
    {
        fclose (pFileStream0);
        pFileStream0 = NULL;
    }
    if ((pFileStream1) && (iLogMode & CLOSELOG))
    {
        fclose (pFileStream1);
        pFileStream1 = NULL;
    }
    */

    // return error code or OK
    if (iLogMode & ERRORMSG)
    {
        return iErrNum;
    }
    else
    {
        return 0;
    }
}

// *********************************************************************
// Function to log standardized information to console, file or both
int shrLogEx(int iLogMode = LOGCONSOLE, int iErrNum = 0, const char* cFormatString = "", ...)
{
    va_list vaArgList;

    // Prepare variable agument list
    va_start(vaArgList, cFormatString);
    int ret = shrLogV(iLogMode, iErrNum, cFormatString, vaArgList);

    // end variable argument handler
    va_end(vaArgList);

    return ret;
}

// *********************************************************************
// Function to log standardized information to console, file or both
int shrLog(const char* cFormatString = "", ...)
{
    va_list vaArgList;

    // Prepare variable agument list
    va_start(vaArgList, cFormatString);
    //int ret = shrLogV(LOGBOTH, 0, cFormatString, vaArgList);
    int ret = shrLogV(LOGCONSOLE, 0, cFormatString, vaArgList);

    // end variable argument handler
    va_end(vaArgList);

    return ret;
}
