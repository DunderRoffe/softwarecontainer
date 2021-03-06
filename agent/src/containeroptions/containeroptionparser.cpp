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

#include "containeroptions/containeroptionparser.h"

#include "jsonparser.h"

namespace softwarecontainer {

ContainerOptionParser::ContainerOptionParser() :
    m_options(new DynamicContainerOptions())
{
}

void ContainerOptionParser::readConfigElement(const json_t *element)
{
    if (!json_is_object(element)) {
        std::string errorMessage("Configure entry is not an object");
        log_error() << errorMessage;
        throw ContainerOptionParseError(errorMessage);
    }

    bool wo = false;
    if(!JSONParser::read(element, "enableWriteBuffer", wo)) {
        std::string errorMessage("Could not parse config due to: 'enableWriteBuffer' not found.");
        log_error() << errorMessage;
        throw ContainerOptionParseError(errorMessage);
    }

    m_options->setEnableWriteBuffer(wo);
}

std::unique_ptr<DynamicContainerOptions> ContainerOptionParser::parse(const std::string &config)
{
    if (config.size() == 0) {
        std::string errorMessage("Empty JSON config strings are not supported.");
        log_error() << errorMessage;
        throw ContainerOptionParseError(errorMessage);
    }

    json_error_t error;
    json_t *root = json_loads(config.c_str(), 0, &error);

    if (!root) {
        std::string errorMessage("Could not parse config: "
                                + std::string(error.text)
                                + config);
        log_error() << errorMessage;
        throw ContainerOptionParseError(errorMessage);
    }

    if (!json_is_array(root)) {
        std::string errorMessage("Root JSON element is not an array");
        log_error() << errorMessage;
        json_decref(root);
        throw ContainerOptionParseError(errorMessage);
    }

    size_t index;
    json_t *element;

    try {
        json_array_foreach(root, index, element) {
            readConfigElement(element);
        }
    } catch (ContainerOptionParseError &err) {
        json_decref(root);
        throw;
    }
    json_decref(root);

    std::unique_ptr<DynamicContainerOptions> ret = std::move(m_options);
    m_options.reset(new DynamicContainerOptions());
    return ret;
}

} // namespace softwarecontainer
