/*
 * Copyright (c) 2024, Tim Flynn <trflynn89@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/OwnPtr.h>
#include <AK/Variant.h>
#include <AK/Vector.h>
#include <LibGfx/Point.h>
#include <LibIPC/Forward.h>
#include <LibWeb/HTML/SelectedFile.h>
#include <LibWeb/PixelUnits.h>
#include <LibWeb/UIEvents/KeyCode.h>
#include <LibWeb/UIEvents/MouseButton.h>

namespace Web {

struct BrowserInputData {
    virtual ~BrowserInputData() = default;
};

struct KeyEvent {
    enum class Type {
        KeyDown,
        KeyUp,
    };

    KeyEvent clone_without_browser_data() const;

    Type type;
    UIEvents::KeyCode key { UIEvents::KeyCode::Key_Invalid };
    UIEvents::KeyModifier modifiers { UIEvents::KeyModifier::Mod_None };
    u32 code_point { 0 };
    bool repeat { false };

    OwnPtr<BrowserInputData> browser_data;
};

struct MouseEvent {
    enum class Type {
        MouseDown,
        MouseUp,
        MouseMove,
        MouseLeave,
        MouseWheel,
        DoubleClick,
    };

    MouseEvent clone_without_browser_data() const;

    Type type;
    Web::DevicePixelPoint position;
    Web::DevicePixelPoint screen_position;
    UIEvents::MouseButton button { UIEvents::MouseButton::None };
    UIEvents::MouseButton buttons { UIEvents::MouseButton::None };
    UIEvents::KeyModifier modifiers { UIEvents::KeyModifier::Mod_None };
    int wheel_delta_x { 0 };
    int wheel_delta_y { 0 };

    OwnPtr<BrowserInputData> browser_data;
};

struct DragEvent {
    enum class Type {
        DragStart,
        DragMove,
        DragEnd,
        Drop,
    };

    DragEvent clone_without_browser_data() const;

    Type type;
    Web::DevicePixelPoint position;
    Web::DevicePixelPoint screen_position;
    UIEvents::MouseButton button { UIEvents::MouseButton::None };
    UIEvents::MouseButton buttons { UIEvents::MouseButton::None };
    UIEvents::KeyModifier modifiers { UIEvents::KeyModifier::Mod_None };
    Vector<HTML::SelectedFile> files;

    OwnPtr<BrowserInputData> browser_data;
};

using InputEvent = Variant<KeyEvent, MouseEvent, DragEvent>;

struct QueuedInputEvent {
    u64 page_id { 0 };
    InputEvent event;
    size_t coalesced_event_count { 0 };
};

}

namespace IPC {

template<>
ErrorOr<void> encode(Encoder&, Web::KeyEvent const&);

template<>
ErrorOr<Web::KeyEvent> decode(Decoder&);

template<>
ErrorOr<void> encode(Encoder&, Web::MouseEvent const&);

template<>
ErrorOr<Web::MouseEvent> decode(Decoder&);

template<>
ErrorOr<void> encode(Encoder&, Web::DragEvent const&);

template<>
ErrorOr<Web::DragEvent> decode(Decoder&);

}
