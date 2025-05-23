/*
 * Copyright (c) 2024, Aliaksandr Kalenik <kalenik.aliaksandr@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibGC/Heap.h>
#include <LibJS/Runtime/Realm.h>
#include <LibWeb/Bindings/Intrinsics.h>
#include <LibWeb/Bindings/PopStateEventPrototype.h>
#include <LibWeb/HTML/PopStateEvent.h>

namespace Web::HTML {

GC_DEFINE_ALLOCATOR(PopStateEvent);

[[nodiscard]] GC::Ref<PopStateEvent> PopStateEvent::create(JS::Realm& realm, FlyString const& event_name, PopStateEventInit const& event_init)
{
    return realm.create<PopStateEvent>(realm, event_name, event_init);
}

GC::Ref<PopStateEvent> PopStateEvent::construct_impl(JS::Realm& realm, FlyString const& event_name, PopStateEventInit const& event_init)
{
    return realm.create<PopStateEvent>(realm, event_name, event_init);
}

PopStateEvent::PopStateEvent(JS::Realm& realm, FlyString const& event_name, PopStateEventInit const& event_init)
    : DOM::Event(realm, event_name, event_init)
    , m_state(event_init.state)
{
}

void PopStateEvent::initialize(JS::Realm& realm)
{
    WEB_SET_PROTOTYPE_FOR_INTERFACE(PopStateEvent);
    Base::initialize(realm);
}

void PopStateEvent::visit_edges(JS::Cell::Visitor& visitor)
{
    Base::visit_edges(visitor);
    visitor.visit(m_state);
}

}
