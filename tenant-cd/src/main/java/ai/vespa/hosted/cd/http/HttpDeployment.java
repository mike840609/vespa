package ai.vespa.hosted.cd.http;

import ai.vespa.hosted.api.Authenticator;
import ai.vespa.hosted.cd.Deployment;
import ai.vespa.hosted.cd.Endpoint;
import ai.vespa.hosted.cd.TestDeployment;

import java.net.URI;
import java.util.Map;
import java.util.stream.Collectors;

/**
 * A remote deployment of a Vespa application, reachable over HTTP. Contains {@link HttpEndpoint}s.
 *
 * @author jonmv
 */
public class HttpDeployment implements Deployment {

    private final Map<String, HttpEndpoint> endpoints;

    /** Creates a representation of the given deployment endpoints, using the authenticator for data plane access. */
    public HttpDeployment(Map<String, URI> endpoints, Authenticator authenticator) {
        this.endpoints = endpoints.entrySet().stream()
                .collect(Collectors.toUnmodifiableMap(entry -> entry.getKey(),
                                                      entry -> new HttpEndpoint(entry.getValue(), authenticator)));
    }

    @Override
    public Endpoint endpoint() {
        return null;
    }

    @Override
    public Endpoint endpoint(String id) {
        return null;
    }

    @Override
    public TestDeployment asTestDeployment() {
        return null;
    }

}
