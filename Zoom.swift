import Foundation

private let maxEntries = 512
private var entries: [String: Double] = [:]
private var filepath = ""

private func hostString(_ host: UnsafePointer<CChar>?) -> String? {
    guard let host else { return nil }
    let s = String(cString: host)
    return s.isEmpty ? nil : s
}

private func quoted(_ s: String) -> String {
    var out = "\""
    for c in s {
        if c == "\\" {
            out += "\\\\"
        } else if c == "\"" {
            out += "\\\""
        } else {
            out.append(c)
        }
    }
    return out + "\""
}

private func zoomSave() {
    if filepath.isEmpty { return }
    storage_ensure_dir()
    let body = entries.map {
        "\(quoted($0.key)):\(String(format: "%.2f", $0.value))"
    }.joined(separator: ",")
    try? "{\(body)}\n".write(toFile: filepath, atomically: false, encoding: .utf8)
}

@_cdecl("zoom_load")
public func zoom_load(_ path: UnsafePointer<CChar>?) {
    filepath = path.map { String(cString: $0) } ?? ""
    entries.removeAll(keepingCapacity: true)

    guard !filepath.isEmpty,
          let data = try? Data(contentsOf: URL(fileURLWithPath: filepath)),
          let object = try? JSONSerialization.jsonObject(with: data),
          let map = object as? [String: Any] else { return }

    for (host, value) in map {
        if entries.count >= maxEntries { break }
        if host.isEmpty || value is Bool { continue }
        guard let number = value as? NSNumber else { continue }
        let zoom = number.doubleValue
        if zoom > 0.0 { entries[host] = zoom }
    }
}

@_cdecl("zoom_get")
public func zoom_get(_ host: UnsafePointer<CChar>?) -> Double {
    guard let host = hostString(host) else { return 0.0 }
    return entries[host] ?? 0.0
}

@_cdecl("zoom_set")
public func zoom_set(_ host: UnsafePointer<CChar>?, _ z: Double) {
    guard let host = hostString(host) else { return }

    if abs(z - 1.0) < 0.001 {
        if entries.removeValue(forKey: host) != nil { zoomSave() }
        return
    }

    if entries[host] == nil && entries.count >= maxEntries { return }
    entries[host] = z
    zoomSave()
}
