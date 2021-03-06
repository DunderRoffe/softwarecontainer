/*
 * Copyright (C) 2016-2017 Pelagicore AB
 *
 * Permission to use, copy, modify, and/or distribute this software for
 * any purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR
 * BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 * For further information see LICENSE
 */

#include "softwarecontainer-common.h"
#include "softwarecontainererror.h"

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdarg>

#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <libgen.h>

namespace softwarecontainer {
    LOG_DECLARE_DEFAULT_CONTEXT(defaultLogContext, "MAIN", "Main context");

bool getStat(const std::string &path, struct stat &st)
{
    return stat(path.c_str(), &st) == 0;
}

bool isDirectory(const std::string &path)
{
    struct stat st;
    if (getStat(path, st)) {
        return S_ISDIR(st.st_mode);
    }

    return false;
}

bool isDirectoryEmpty(const std::string &path) {
    int n = 0;
    constexpr int EMPTY_DIR_SIZE=2;
    struct dirent *d;
    DIR *dir = opendir(path.c_str());
    if (dir == NULL) { //Not a directory or doesn't exist
        return true;
    }
    while ((d = readdir(dir)) != NULL) {
        ++n;
        if(n > EMPTY_DIR_SIZE) {
            break;
        }
    }
    closedir(dir);
    if (n <= EMPTY_DIR_SIZE) { //Directory Empty
        return true;
    } else {
        return false;
    }
}

bool isFile(const std::string &path)
{
    struct stat st;
    if (getStat(path, st)) {
        return S_ISREG(st.st_mode);
    }

    return false;
}

bool isPipe(const std::string &path)
{
    struct stat st;
    if (getStat(path, st)) {
        return S_ISFIFO(st.st_mode);
    }

    return false;
}

bool isSocket(const std::string &path)
{
    struct stat st;
    if (getStat(path, st)) {
        return S_ISSOCK(st.st_mode);
    }

    return false;
}

bool existsInFileSystem(const std::string &path)
{
    struct stat st;
    return getStat(path, st);
}

std::string parentPath(const std::string &path_)
{
    char *path = strdup(path_.c_str());
    if (nullptr == path) {
        std::string message = "Failed to do strdup on " + path_;
        log_error() << message;
        throw SoftwareContainerError(message);
    }

    std::string parent = dirname(path);
    free(path);
    return parent;
}

std::string baseName(const std::string &path)
{
    return Glib::path_get_basename(path);
}

std::string buildPath(const std::string &arg1, const std::string &arg2)
{
    return Glib::build_filename(arg1, arg2);
}

std::string buildPath(const std::string &arg1, const std::string &arg2, const std::string &arg3)
{
    return buildPath(buildPath(arg1, arg2), arg3);
}

bool touch(const std::string &path)
{
    int fd = open(path.c_str(), O_WRONLY | O_CREAT | O_NOCTTY | O_NONBLOCK | O_LARGEFILE, 0666);
    if (fd != -1) {
        close(fd);
        return true;
    } else {
        return false;
    }
}

bool writeToFile(const std::string &path, const std::string &content)
{
    bool ret = true;
    log_verbose() << "writing to " << path << " : " << content;
    std::ofstream out(path);
    if (out.is_open()) {
        out << content;
        if (!out.good()) {
            ret = false;
        }
        out.close();
    } else {
        ret = false;
    }
    return ret;
}

bool readFromFile(const std::string &path, std::string &content)
{
    bool ret = true;
    std::ifstream t(path);
    if (t.is_open()) {
        std::stringstream buffer;
        buffer << t.rdbuf();
        content = buffer.str();
        if (!t.good()) {
            ret = false;
        }
        t.close();
    } else {
        ret = false;
    }
    return ret;
}

bool parseInt(const char *arg, int *result)
{
    char *end;
    long value = strtol(arg, &end, 10);
    if (end == arg || *end != '\0' || errno == ERANGE) {
        return false;
    }

    *result = value;
    return true;
}

} // namespace softwarecontainer
