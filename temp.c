int write_status_line(REQUEST_INFO* ri_requestInfo, char* buffer, size_t iOffset)
{
    if (!ri_requestInfo || !buffer) return -1;


    /* Status line format -> "version status reason\r\n" */

    /* choose a sensible HTTP version: echo valid request version or fall back */
    const char* version = "HTTP/1.1";
    if (ri_requestInfo->m_szVersion &&
        (!strcmp(ri_requestInfo->m_szVersion, "HTTP/1.0") ||
         !strcmp(ri_requestInfo->m_szVersion, "HTTP/1.1")))
    {
        version = ri_requestInfo->m_szVersion;
    }

    /* decide status code:
       - if parser succeeded, assume 200 OK (application can override by not using this helper)
       - if parser failed, map parse result to an appropriate error status */
    int status = (ri_requestInfo->m_parseResult == PARSE_SUCCESS) ?
                 200 : parse_result_to_http_status(ri_requestInfo->m_parseResult);

    const char* reason = (status == 200) ? "OK" : http_reason_phrase(status);

    /* remaining space in the header buffer */
    size_t remaining = MAX_RESPONSE_HEADER_SIZE > iOffset ? MAX_RESPONSE_HEADER_SIZE - iOffset : 0;
    if (remaining == 0) return -1;

    /* snprintf returns the number of characters that would have been written (excluding '\0') */
    int wrote = snprintf(buffer + iOffset, remaining, "%s %d %s\r\n", version, status, reason);
    if (wrote < 0) return -1;

    /* if wrote >= remaining, the output was truncated -> error */
    if ((size_t)wrote >= remaining) return -1;

    /* return new offset (old + bytes written) */
    return (int)(iOffset + (size_t)wrote);
}
