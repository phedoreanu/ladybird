/*
 * Copyright (c) 2022-2025, Sam Atkins <sam@ladybird.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Math.h>
#include <LibWeb/CSS/Angle.h>
#include <LibWeb/CSS/Percentage.h>
#include <LibWeb/CSS/StyleValues/CalculatedStyleValue.h>

namespace Web::CSS {

Angle::Angle(double value, Type type)
    : m_type(type)
    , m_value(value)
{
}

Angle Angle::make_degrees(double value)
{
    return { value, Type::Deg };
}

Angle Angle::percentage_of(Percentage const& percentage) const
{
    return Angle { percentage.as_fraction() * m_value, m_type };
}

String Angle::to_string(SerializationMode serialization_mode) const
{
    if (serialization_mode == SerializationMode::ResolvedValue)
        return MUST(String::formatted("{}deg", to_degrees()));
    return MUST(String::formatted("{}{}", raw_value(), unit_name()));
}

double Angle::to_degrees() const
{
    switch (m_type) {
    case Type::Deg:
        return m_value;
    case Type::Grad:
        return m_value * (360.0 / 400.0);
    case Type::Rad:
        return AK::to_degrees(m_value);
    case Type::Turn:
        return m_value * 360.0;
    }
    VERIFY_NOT_REACHED();
}

double Angle::to_radians() const
{
    return AK::to_radians(to_degrees());
}

StringView Angle::unit_name() const
{
    switch (m_type) {
    case Type::Deg:
        return "deg"sv;
    case Type::Grad:
        return "grad"sv;
    case Type::Rad:
        return "rad"sv;
    case Type::Turn:
        return "turn"sv;
    }
    VERIFY_NOT_REACHED();
}

Optional<Angle::Type> Angle::unit_from_name(StringView name)
{
    if (name.equals_ignoring_ascii_case("deg"sv)) {
        return Type::Deg;
    }
    if (name.equals_ignoring_ascii_case("grad"sv)) {
        return Type::Grad;
    }
    if (name.equals_ignoring_ascii_case("rad"sv)) {
        return Type::Rad;
    }
    if (name.equals_ignoring_ascii_case("turn"sv)) {
        return Type::Turn;
    }
    return {};
}

Angle Angle::resolve_calculated(NonnullRefPtr<CalculatedStyleValue const> const& calculated, Layout::Node const& layout_node, Angle const& reference_value)
{
    return calculated->resolve_angle_deprecated(
                         {
                             .percentage_basis = reference_value,
                             .length_resolution_context = Length::ResolutionContext::for_layout_node(layout_node),
                         })
        .value();
}

}
