/***************************************************************************
 *
 * Project         _____    __   ____   _      _
 *                (  _  )  /__\ (_  _)_| |_  _| |_
 *                 )(_)(  /(__)\  )( (_   _)(_   _)
 *                (_____)(__)(__)(__)  |_|    |_|
 *
 *
 * Copyright 2018-present, Leonid Stryzhevskyi <lganzzzo@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***************************************************************************/

#include "Serializer2.hpp"

#include "oatpp/parser/json/Utils.hpp"

namespace oatpp { namespace parser { namespace json { namespace mapping {

Serializer2::Serializer2(const std::shared_ptr<Config>& config)
  : m_config(config)
{

  m_methods.resize(data::mapping::type::ClassId::getClassCount(), nullptr);

  m_methods[oatpp::data::mapping::type::__class::String::CLASS_ID.id] = &Serializer2::serializeString;
  m_methods[oatpp::data::mapping::type::__class::Int8::CLASS_ID.id] = &Serializer2::serializePrimitive<oatpp::Int8>;
  m_methods[oatpp::data::mapping::type::__class::Int16::CLASS_ID.id] = &Serializer2::serializePrimitive<oatpp::Int16>;
  m_methods[oatpp::data::mapping::type::__class::Int32::CLASS_ID.id] = &Serializer2::serializePrimitive<oatpp::Int32>;
  m_methods[oatpp::data::mapping::type::__class::Int64::CLASS_ID.id] = &Serializer2::serializePrimitive<oatpp::Int64>;
  m_methods[oatpp::data::mapping::type::__class::Float32::CLASS_ID.id] = &Serializer2::serializePrimitive<oatpp::Float32>;
  m_methods[oatpp::data::mapping::type::__class::Float64::CLASS_ID.id] = &Serializer2::serializePrimitive<oatpp::Float64>;
  m_methods[oatpp::data::mapping::type::__class::Boolean::CLASS_ID.id] = &Serializer2::serializePrimitive<oatpp::Boolean>;

  m_methods[oatpp::data::mapping::type::__class::AbstractObject::CLASS_ID.id] = &Serializer2::serializeObject;
  m_methods[oatpp::data::mapping::type::__class::AbstractList::CLASS_ID.id] = &Serializer2::serializeList;
  m_methods[oatpp::data::mapping::type::__class::AbstractListMap::CLASS_ID.id] = &Serializer2::serializeFieldsMap;

}

void Serializer2::setSerializerMethod(const data::mapping::type::ClassId& classId, SerializerMethod method) {
  auto id = classId.id;
  if(id < m_methods.size()) {
    m_methods[id] = method;
  } else {
    throw std::runtime_error("[oatpp::parser::json::mapping::Serializer::setSerializerMethod()]: Error. Unknown classId");
  }
}

void Serializer2::serializeString(oatpp::data::stream::ConsistentOutputStream* stream, p_char8 data, v_buff_size size) {
  auto encodedValue = Utils::escapeString(data, size, false);
  stream->writeCharSimple('\"');
  stream->writeSimple(encodedValue);
  stream->writeCharSimple('\"');
}

void Serializer2::serializeString(Serializer2* serializer,
                                  data::stream::ConsistentOutputStream* stream,
                                  const data::mapping::type::AbstractObjectWrapper& polymorph)
{

  if(!polymorph) {
    stream->writeSimple("null", 4);
    return;
  }

  auto str = static_cast<oatpp::base::StrBuffer*>(polymorph.get());

  serializeString(stream, str->getData(), str->getSize());

}

void Serializer2::serializeList(Serializer2* serializer,
                                data::stream::ConsistentOutputStream* stream,
                                const data::mapping::type::AbstractObjectWrapper& polymorph)
{

  if(!polymorph) {
    stream->writeSimple("null", 4);
    return;
  }

  auto* list = static_cast<AbstractList*>(polymorph.get());

  stream->writeCharSimple('[');
  bool first = true;
  auto curr = list->getFirstNode();

  while(curr != nullptr){
    auto value = curr->getData();
    if(value || serializer->getConfig()->includeNullFields) {
      (first) ? first = false : stream->writeSimple(",", 1);
      serializer->serialize(stream, curr->getData());
    }
    curr = curr->getNext();
  }

  stream->writeCharSimple(']');

}

void Serializer2::serializeFieldsMap(Serializer2* serializer,
                                     data::stream::ConsistentOutputStream* stream,
                                     const data::mapping::type::AbstractObjectWrapper& polymorph)
{

  if(!polymorph) {
    stream->writeSimple("null", 4);
    return;
  }

  auto map = static_cast<AbstractFieldsMap*>(polymorph.get());

  stream->writeCharSimple('{');
  bool first = true;
  auto curr = map->getFirstEntry();

  while(curr != nullptr){
    auto value = curr->getValue();
    if(value || serializer->getConfig()->includeNullFields) {
      (first) ? first = false : stream->writeSimple(",", 1);
      auto key = curr->getKey();
      serializeString(stream, key->getData(), key->getSize());
      stream->writeSimple(":", 1);
      serializer->serialize(stream, curr->getValue());
    }
    curr = curr->getNext();
  }

  stream->writeCharSimple('}');

}

void Serializer2::serializeObject(Serializer2* serializer,
                                  data::stream::ConsistentOutputStream* stream,
                                  const data::mapping::type::AbstractObjectWrapper& polymorph)
{

  if(!polymorph) {
    stream->writeSimple("null", 4);
    return;
  }

  stream->writeCharSimple('{');

  bool first = true;
  auto fields = polymorph.valueType->properties->getList();
  Object* object = static_cast<Object*>(polymorph.get());

  for (auto const& field : fields) {

    auto value = field->get(object);
    if(value || serializer->getConfig()->includeNullFields) {
      (first) ? first = false : stream->writeSimple(",", 1);
      serializeString(stream, (p_char8)field->name, std::strlen(field->name));
      stream->writeSimple(":", 1);
      serializer->serialize(stream, value);
    }

  }

  stream->writeCharSimple('}');

}

void Serializer2::serialize(data::stream::ConsistentOutputStream* stream,
                            const data::mapping::type::AbstractObjectWrapper& polymorph)
{
  auto id = polymorph.valueType->classId.id;
  auto& method = m_methods[id];
  if(method) {
    (*method)(this, stream, polymorph);
  } else {
    throw std::runtime_error("[oatpp::parser::json::mapping::Serializer::serialize()]: "
                             "Error. No serialize method for type '" + std::string(polymorph.valueType->classId.name) + "'");
  }
}

const std::shared_ptr<Serializer2::Config>& Serializer2::getConfig() {
  return m_config;
}

}}}}
