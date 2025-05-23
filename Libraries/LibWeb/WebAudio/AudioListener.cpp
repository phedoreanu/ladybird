/*
 * Copyright (c) 2024, Jelle Raaijmakers <jelle@ladybird.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibGC/CellAllocator.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/WebAudio/AudioListener.h>

namespace Web::WebAudio {

GC_DEFINE_ALLOCATOR(AudioListener);

AudioListener::AudioListener(JS::Realm& realm, GC::Ref<BaseAudioContext> context)
    : Bindings::PlatformObject(realm)
    , m_forward_x(AudioParam::create(realm, context, 0.f, NumericLimits<float>::lowest(), NumericLimits<float>::max(), Bindings::AutomationRate::ARate))
    , m_forward_y(AudioParam::create(realm, context, 0.f, NumericLimits<float>::lowest(), NumericLimits<float>::max(), Bindings::AutomationRate::ARate))
    , m_forward_z(AudioParam::create(realm, context, -1.f, NumericLimits<float>::lowest(), NumericLimits<float>::max(), Bindings::AutomationRate::ARate))
    , m_position_x(AudioParam::create(realm, context, 0.f, NumericLimits<float>::lowest(), NumericLimits<float>::max(), Bindings::AutomationRate::ARate))
    , m_position_y(AudioParam::create(realm, context, 0.f, NumericLimits<float>::lowest(), NumericLimits<float>::max(), Bindings::AutomationRate::ARate))
    , m_position_z(AudioParam::create(realm, context, 0.f, NumericLimits<float>::lowest(), NumericLimits<float>::max(), Bindings::AutomationRate::ARate))
    , m_up_x(AudioParam::create(realm, context, 0.f, NumericLimits<float>::lowest(), NumericLimits<float>::max(), Bindings::AutomationRate::ARate))
    , m_up_y(AudioParam::create(realm, context, 1.f, NumericLimits<float>::lowest(), NumericLimits<float>::max(), Bindings::AutomationRate::ARate))
    , m_up_z(AudioParam::create(realm, context, 0.f, NumericLimits<float>::lowest(), NumericLimits<float>::max(), Bindings::AutomationRate::ARate))
{
}

GC::Ref<AudioListener> AudioListener::create(JS::Realm& realm, GC::Ref<BaseAudioContext> context)
{
    return realm.create<AudioListener>(realm, context);
}

AudioListener::~AudioListener() = default;

// https://webaudio.github.io/web-audio-api/#dom-audiolistener-setposition
WebIDL::ExceptionOr<void> AudioListener::set_position(float x, float y, float z)
{
    // This method is DEPRECATED. It is equivalent to setting positionX.value, positionY.value, and
    // positionZ.value directly with the given x, y, and z values, respectively.

    // FIXME: Consequently, any of the positionX, positionY, and positionZ AudioParams for this
    //        AudioListener have an automation curve set using setValueCurveAtTime() at the time this
    //        method is called, a NotSupportedError MUST be thrown.

    m_position_x->set_value(x);
    m_position_y->set_value(y);
    m_position_z->set_value(z);

    return {};
}

// https://webaudio.github.io/web-audio-api/#dom-audiolistener-setorientation
WebIDL::ExceptionOr<void> AudioListener::set_orientation(float x, float y, float z, float x_up, float y_up, float z_up)
{
    // This method is DEPRECATED. It is equivalent to setting forwardX.value, forwardY.value,
    // forwardZ.value, upX.value, upY.value, and upZ.value directly with the given x, y, z, xUp,
    // yUp, and zUp values, respectively.

    // FIXME: Consequently, if any of the forwardX, forwardY, forwardZ, upX, upY and upZ
    //        AudioParams have an automation curve set using setValueCurveAtTime() at the time this
    //        method is called, a NotSupportedError MUST be thrown.

    m_forward_x->set_value(x);
    m_forward_y->set_value(y);
    m_forward_z->set_value(z);
    m_up_x->set_value(x_up);
    m_up_y->set_value(y_up);
    m_up_z->set_value(z_up);

    return {};
}

void AudioListener::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(AudioListener);
    Base::initialize(realm);
}

void AudioListener::visit_edges(Cell::Visitor& visitor)
{
    Base::visit_edges(visitor);
    visitor.visit(m_forward_x);
    visitor.visit(m_forward_y);
    visitor.visit(m_forward_z);
    visitor.visit(m_position_x);
    visitor.visit(m_position_y);
    visitor.visit(m_position_z);
    visitor.visit(m_up_x);
    visitor.visit(m_up_y);
    visitor.visit(m_up_z);
}

}
